/*
 * Copyright (c) 2003 Communication Networks Lab, Simon Fraser University.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors: Tony Dongliang Feng <tdfeng@cs.sfu.ca>, Brian J. Premore.
 *
 */

 
#include "peer-entry.h"
#include "rtProtoBGP.h"
                                         
static class PeerEntryclass : public TclClass {
public:
	PeerEntryclass() : TclClass("Agent/PeerEntry") {}
	TclObject* create(int, const char*const*) {
		return (new PeerEntry);
	}
} class_peer_entry;                  
                     
PeerEntry::PeerEntry(): Agent(PT_PEERENTRY) {
	bind ("ipaddr_",&ip_addr_);
	bind ("as_num_",&as_num);
	bind ("return_ipaddr_",&return_ipaddr_);
	bind ("bgp_id_",&bgp_id_);
	bind ("hold_time_",&hold_timer_interval);
	bind ("keep_alive_interval_",&keep_alive_interval);
	bind ("mrai_",&mrai);
  
  readsocket = NULL;
  writesocket = NULL;
  ht = NULL;
  crt = NULL;
  ka = NULL;
  subtyp = NONCLIENT;      

  if (Global::user_hold_time_default != -1)
     hold_timer_interval = Global::user_hold_time_default;
  if (Global::user_keep_alive_time_default != -1)
     keep_alive_interval = Global::user_keep_alive_time_default;
  if (Global::user_ebgp_mrai_default != -1)
     mrai = Global::user_ebgp_mrai_default;
  
}		

int PeerEntry::command(int argc, const char*const* argv)
{
  if (strcmp(argv[1], "set-internal") == 0) {
		typ = INTERNAL;
		return TCL_OK;
	}
  if (strcmp(argv[1], "set-external") == 0) {
		typ = EXTERNAL;
		return TCL_OK;
	}
  if (strcmp(argv[1], "set-client") == 0) {
    typ = INTERNAL;  // client implies internal
		subtyp = CLIENT;
		return TCL_OK;
	}

	return Agent::command(argc, argv);
}

  // ----- PeerEntry::equals ------------------------------------------------ //
  /**
   * Determines whether two peer entries are equal.  They are equal only if
   * their NHI prefixes are equal.
   *
   * @param pe  The peer entry with which to make the comparison.
   * @return whether or not the two peer entries are equal
   */
bool PeerEntry::equals(TclObject * pe) {
    if ( pe == NULL ) {
      return false;
    }  else {
      return ( typeid(*pe) == typeid(PeerEntry) &&
                ((PeerEntry*)pe)->as_num == as_num &&
                ((PeerEntry*)pe)->ip_addr_ == ip_addr_);
    }
}

  // ----- PeerEntry::connect ----------------------------------------------- //
  /**
   * Attempts to establish a socket connection with this peer.
   */
void PeerEntry::connect() {
    writesocket = new TcpSocket();
    writesocket->setTcpMaster(bgp->tcpmaster);
    writesocket->bind(return_ip->intval(), bgp->PORT_NUM);
    writing[writesocket]=false;
    writeconnecting[writesocket]=true;
    writeconnected[writesocket]=false;
    
    class ConnectCon : public Continuation  {
    private:
        TcpSocket *mywritesocket;
    public:
        ConnectCon (PeerEntry * peer) : Continuation(peer) {} 
        ~ConnectCon() {}
        inline  virtual void success () {
            mywritesocket = peer->writesocket;
            peer->writeconnected[mywritesocket] = true;
            peer->writeconnecting[mywritesocket]= false;
            // Now we have a channel to write on.  However, a full transport
            // connection for BGP consists of two Socket/TCP connections--one
            // on which to send outgoing messages, and another on which to
            // listen for incoming messages.  If we already have the incoming
            // connection, then we're done.  If not, we must establish that
            // connection.  (It's also possible that something happened during
            // the connection process that required the connection to be
            // closed.  We must first check for that before indicating that the
            // transport connection is opened.)
            if (mywritesocket == peer->writesocket) {
              // no new write socket while connecting
              if (peer->readsocket != NULL) { // incoming connection already exists
                 peer->bgp->push(new TransportMessage(rtProtoBGP::TransConnOpen, peer->nh));
               }   // else we just listen to hear back from the peer in order to
                   // establish the other connection
            } else { // connection was aborted
               printf("malfunct socket! \n");
               peer->write_close(mywritesocket);
            }
        }
        inline virtual void failure () {
            peer->writeconnecting[mywritesocket]=false;
            if (mywritesocket == peer->writesocket) {
                peer->bgp->push(new TransportMessage(rtProtoBGP::TransConnOpenFail, peer->nh));

            } else { // connection was aborted
                printf("malfunct socket! \n");
                peer->write_close(mywritesocket);
            }
          }
    }; // end of class ConnectCon

    ConnectCon * conCon = new ConnectCon(this);
    writesocket->connect(ip_addr->intval(), bgp->PORT_NUM, conCon);
}

 // ----- PeerEntry::receive ----------------------------------------------- //
  /**
   * Receives BGP messages from the socket connection with this peer.
   */
void PeerEntry::receive() {
    class ReadBodyCon : public Continuation  {
    private:
        TcpSocket * myreadsocket;
        char * header;
        char * body;
    public:
        ReadBodyCon (PeerEntry * peer, char * h, char * b) : Continuation(peer) {
          myreadsocket = peer->readsocket;
          header = h;
          body = b;
        }   
        ~ReadBodyCon() {}

        inline  virtual void success () {
           if (myreadsocket == peer->readsocket) { // session still open             
              // Create a new message from the stream of the message header and body.
              int type = BGPMessage::type_fromheader(header );
              int msgLength =  BGPMessage::bytecount_fromheader(header);
              unsigned char * strMsg = new unsigned char[msgLength];
              for (int i=0; i<BGPMessage::OCTETS_IN_HEADER; i++) {
                strMsg[i] = header[i];
              }
              for (int j=BGPMessage::OCTETS_IN_HEADER,k=0; j<msgLength; j++,k++) {
                strMsg[j] = body[k];
              }
              BGPMessage * msg;
              switch (type) {
                  case BGPMessage::OPEN :
                  msg = new OpenMessage(strMsg,peer->nh);
                  break;
                  case BGPMessage::UPDATE :
                  msg = new UpdateMessage(strMsg,peer->nh);
                  break;
                  case BGPMessage::NOTIFICATION :
                  msg = new NotificationMessage(strMsg,peer->nh);
                  break;
                  case BGPMessage::KEEPALIVE :
                  msg = new KeepAliveMessage(peer->nh);
                  break;
                  default:
                    printf( "Received invalid type of message. \n");
                    return;
              }
              // free up memory used by the stream.
              delete[] header;
              delete[] body;
              delete[] strMsg;
              peer->bgp->push((BGPMessage *)msg);
              if (peer->readsocket != NULL) {
                 // push() could result in readsocket becoming null (if a
                 // Notification is being handled)
                 peer->receive();
              } 
           } else { // session closed during read; this is defunct socket
              if (peer->readsocket != NULL) {
                 peer->reading[peer->readsocket] = false;
                 peer->read_close(myreadsocket);
              }
           }
        }
        inline virtual void failure () {
  	        printf( "Failed to read message body. \n");
        }
    }; // end of class ReadbodyCon

  class ReadCon : public Continuation  {
    private:
        TcpSocket *myreadsocket;
        char * header;
    public:
        ReadCon (PeerEntry * peer, char * h ) : Continuation(peer) {
          header = h;
        }
        ~ReadCon() {}
        inline  virtual void success () {
          myreadsocket = peer->readsocket;        
          int bodybytes = BGPMessage::body_bytecount_fromheader((char *)header);
          char * body =  new char[bodybytes];
          ReadBodyCon * readBodyCon = new ReadBodyCon(peer, header, body);

          if (bodybytes > 0) {
            myreadsocket->read(body, bodybytes, readBodyCon);
          } else {
            readBodyCon->success();
          }
        }

        inline virtual void failure () {
          if (myreadsocket != peer->readsocket) {
            // this is a defunct read socket
            printf("defunct socket read failure (ignoring). \n");
            return;
          }
          printf("Socket reading failure. \n");
          if (peer->readsocket != NULL) { // peering session still open
            peer->receive();
          }
        }
  }; // end of class ReadCon

  char * header = new char[BGPMessage::OCTETS_IN_HEADER];
  reading[readsocket]=true;
  ReadCon * readCon = new ReadCon(this,header);
  readsocket->read(header, BGPMessage::OCTETS_IN_HEADER, readCon);
}

  // ----- PeerEntry::send -------------------------------------------------- //
  /**
   * Attempt to send a message to this peer.  If the socket is busy, the data
   * to be written will be enqueued until the socket is free.
   *
   * @param msg  A BGP message to be sent to the peer.
   */
void PeerEntry::send(BGPMessage *msg) {    
    if (writing[writesocket] == false) {
      write(msg);
    } else { // enqueue the data to be written later
      writeq.push(msg);
    }
}


  // ----- PeerEntry::write ------------------------------------------------- //
  /**
   * Attempt to write to the current socket connected to this peer.
   *
   * @param msg  A BGP message to be written to the socket.
   */
void PeerEntry::write(BGPMessage *msg) {
    write(msg,writesocket);
}


  // ----- PeerEntry::write ------------------------------------------------- //
  /**
   * Attempt to write to a socket that either is or was connected to this peer.
   * Since it is possible that a new socket is opened before all packets are
   * done being written to an old one (pending closing), we need to give the
   * option of specifying which write socket to write to.
   *
   * @param ws   A socket to write to.
   * @param msg  A BGP message to be written to the socket.
   */
void PeerEntry::write(BGPMessage *msg, TcpSocket *writesock) {
    writing[writesock]=true;

    class WriteCon : public Continuation   {
    private:
        TcpSocket* mywritesocket;
    public:
        WriteCon (PeerEntry * peer, TcpSocket* ws) : Continuation(peer) {
            mywritesocket = ws;
        }
        ~WriteCon() {}
        inline  virtual void success () {
          if (peer->writeq.size() > 0) {
            BGPMessage * next_msg = peer->writeq.front();
            peer->writeq.pop();
            peer->write(next_msg, mywritesocket);
          } else {
            peer->writing[mywritesocket]=false;
            if (mywritesocket != peer->writesocket) {
              // this write socket is no longer the current write socket
              peer->write_close(mywritesocket);
            }
          }
        }
        inline virtual void failure () {
  	      printf( "Socket writing failure. \n");
          // the connection was dropped
          while (peer->writeq.size()>0) {
            peer->writeq.pop(); // remove pending writes
          }
          peer->close(); // close the read socket connection, too
          peer->cancel_timers();
          peer->connected = false;
          peer->connection_state = rtProtoBGP::IDLE;
          if (Global::auto_reconnect) {
              peer->bgp->push(new StartStopMessage(rtProtoBGP::BGPstart,peer->nh));
          }
        }
    };                

    WriteCon * writeCon = new WriteCon(this,writesock);
    char * msg_stream = new char [msg->bytecount()+1];
    msg->toStream((char *)msg_stream);
    writesock->write(msg_stream, msg->bytecount(), writeCon);
    delete msg;
}

  // ----- PeerEntry::close ------------------------------------------------- //
  /**
   * Close socket connections with this peer.
   */
void PeerEntry::close() {
   if (readsocket != NULL) {
        read_close(readsocket);
        readsocket = NULL;
    }
    if (writesocket != NULL) {
      if ((writeconnecting[writesocket] == false ||
           writeconnected[writesocket] == true) &&
           writing[writesocket] == false) {
        // (a) no connection establishment is in progress with write socket and
        // (b) the write socket is not in use (no bytes are being written)
        write_close(writesocket);
      } // else write_close() will be called when outstanding call completes
      writesocket = NULL;
    }
    while (!writeq.empty()) {
      writeq.pop();
    } 
}

  // ----- PeerEntry::read_close -------------------------------------------- //
  /**
   * Close read socket connection with this peer.
   */
void PeerEntry::read_close(TcpSocket *rsocket) {
    class ReadCloseCon : public Continuation {
    private:
        TcpSocket* mywritesocket;
        rtProtoBGP * bgp;
    public:
        ReadCloseCon (PeerEntry * peer,TcpSocket* ws, rtProtoBGP * b) : Continuation(peer) {
            mywritesocket = ws;
            bgp = b;
        }
        ~ReadCloseCon() {}
        inline  virtual void success () {
           if (Global::auto_reconnect)  
              mywritesocket->unbindTcpAgent();
        }
        inline virtual void failure () {
  	        printf("Read Close failure. \n");
        }
    };

    ReadCloseCon * readCloseCon = new ReadCloseCon(this,rsocket,bgp);
    rsocket->close(readCloseCon);
}

  // ----- PeerEntry::write_close ------------------------------------------- //
  /**
   * Close write socket connection with this peer.
   */
void PeerEntry::write_close(TcpSocket *wsocket) {
    writeconnecting[wsocket]=false;
    if (writeconnected[wsocket] == false) {
      // If the socket connection isn't established, trying to close it will
      // cause an error.
      return;
    }

    writeconnected[wsocket]=false;
    class WriteCloseCon : public Continuation   {
    private:
        TcpSocket* mywritesocket;
    public:
        WriteCloseCon (PeerEntry * peer, TcpSocket* ws) : Continuation(peer) {
             mywritesocket = ws;
        }
        ~WriteCloseCon() {}
        inline  virtual void success () {
        }
        inline virtual void failure () {
  	        printf("Write Close failure. \n");
        }
    };

    WriteCloseCon * writeCloseCon = new WriteCloseCon(this,wsocket);
    wsocket->close(writeCloseCon);
    
    writeconnecting.erase(wsocket);
    writeconnected.erase(wsocket);
    writing.erase(wsocket);
}

  // ----- PeerEntry::cancel_timers ----------------------------------------- //
  /**
   * Cancels all timers associated with this peer.
   */
void PeerEntry::cancel_timers() {
    if (crt != NULL) {
      crt->force_cancel();
    }
    if (ht != NULL) {
      ht->force_cancel();
    }
    if (ka != NULL) {
      ka->force_cancel();
    }
    
    if (bgp->rate_limit_by_dest) {
      map<unsigned int, MRAITimer*>::iterator i;
      // After resetting all mrai timers, we remove them from mrais.
      for (i=mrais.begin();i!=mrais.end();i++) {
         if ( i->second != NULL )       
           i->second->reset();
      }   
      mrais.clear();
    } else {
      if (mraiTimer != NULL) 
        mraiTimer->reset();
    }      
}
