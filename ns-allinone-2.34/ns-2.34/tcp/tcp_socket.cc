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
 * Authors: Zheng Wang, Tony Dongliang Feng <tdfeng@cs.sfu.ca>,
 *          Hongbo Liu, Andy Ogielski.
 */

#include "tcp_socket.h"

  /**
   * Creates an unconnected, unnamed TCP socket.
   */
TcpSocket::TcpSocket()
{
	app_call_waiting = NULL;
	accept_call_waiting = NULL;
	appCallWaiting = false;

	isListening = false;
	source_ip_addr = -1;
	socketState = 0;
  myTcpAgent = NULL;
}

TcpSocket::~TcpSocket()
{
	if(myTcpAgent)
		delete myTcpAgent;
}

void TcpSocket::socketInfo(char* info)
{
	cerr << info << endl;
}

  /** Bind the socket to a local port number and IP address. 
   *  A socket user acting as client need not care about local
   *  IP address. In contrast, a well-known local address and port
   *  must be bound to a server socket before it can listen for
   *  connection requests.
   */
void TcpSocket::bind(int src_ip,int src_port)
{
	source_ip_addr = src_ip;
	source_port = src_port;
}  

  /** Bind a tcpAgent to this socket.
   *  Callback from TcpMaster in response to active or passive
   *  open request.
   */
void TcpSocket::bindTcpAgent(FullTcpAgent* agent)
{
	myTcpAgent = agent;
}

  /** Unbind a tcpAgent to this socket.
   *  Will be called when the peer entry using this socket
   *  successfully read_closed.
   */
void TcpSocket::unbindTcpAgent() {
   tcpMaster->removeReadTcpAgent(myTcpAgent);
   myTcpAgent = NULL;
}

  /** Active open a TCP connection.
   *  If this socket already has an open connection, callback
   *  the Continuation with error.
   *  If socket is set as a listening socket, callback the Continuation
   */
void TcpSocket::connect(int remote_ip, int remote_port, Continuation* caller)
{

	if((socketState & SS_ISCONNECTED) != 0) {
      caller->failure(); 
      return;
  }

	dest_ip_addr = remote_ip;
	destination_port = remote_port;

	app_call_waiting = caller;
	appCallWaiting = true;
	tcpMaster->openConnect(destination_port,dest_ip_addr,this);
	if((socketState & SS_NBIO) != 0)
      caller->failure(); 
}

  /** Passive open a TCP agent and listen on the port bound to the socket
   *  for incoming connection requests. 
   */
void TcpSocket::listen()
{
	char info[250];
	if(source_ip_addr < 0 || source_port < 0) 
	{
		strcpy(info,"cannot listen(), socket not bound to a valid IP or port");
		socketInfo(info);
		exit(-1);
    }
    if(isListening) {
      strcpy(info,"Warning: listen() called already");
	  socketInfo(info);
      exit(-1);
    }
    isListening = true;
    tcpMaster->openServer(source_port,this);  
}

/** TCP calls it to indicate a connection has opened successfully */
void TcpSocket::connected()
{
	  socketState ^= SS_ISCONNECTING;
    socketState |= SS_ISCONNECTED;
    if (!appCallWaiting)
	{ 
      socketInfo("socket: no application is connecting");
      return;
    }
    appCallWaiting = false;
    app_call_waiting->success();
}

  /** Accept a socket from the complete socket queue.
   *  Continuation.success() callback is invoked when a complete
   *  socket is available. 
   *  Continuation.failure() callback is invoked if an
   *  error condition is encountered.
   */
void TcpSocket::accept(TcpSocket** newSocket, Continuation* caller)
{
	  if(!isListening) {  
		    socketInfo("error: Cannot accept without listen");
		    exit(0);
    }

    if(complete_sockets.size() > 0){
        // if there is an established connections in complete socket
        // queue, place it in new_socket[0] and call back Continuation. 
        newSocket[0] = (TcpSocket*)complete_sockets.front();
        caller->success(); 
    } else {
        // if no established conections are pending and the socket
        // is non-blocking, return to Continuation.failure().   
        if((socketState & SS_NBIO) != 0)
          caller->failure();  
      else {
        // if no established conections are pending and the socket
        // is blocking, cache new_socket and Continuation caller until
        // an established connection arrives      
        accept_call_waiting = caller;
        accept_socket = newSocket;
      }
    }  
}                       

  /** If this is a listening socket, returns a new socket created in
   *  response to received connection request. The new socket is put
   *  in the incomplete socket queue. 
   *  Called by tcpMaster when a connection request (SYN) arrives
   *  to a passively opened tcp connection.
   */
void TcpSocket::addSocket(TcpSocket * sock, int s_addr, int s_port)
{
 	if(!isListening)
	{
		socketInfo("Socket: non-server socket cannot accept a connection!");
	}
	sock->listeningSocket = this;
  sock->dest_ip_addr = s_addr;
	sock->destination_port = s_port;
	if((socketState & SS_NBIO) != 0)
      sock->socketState |= (SS_NBIO | SS_ISCONNECTING);
	incomplete_sockets.push_back(sock);
}

  /** Informs the listening socket that a pending connection
   *  request has become an established connection ready to
   *  be accepted.
   */
void TcpSocket::addConnection(TcpSocket* socket)
{
	socket->socketState ^= SS_ISCONNECTING;
  socket->socketState |= SS_ISCONNECTED;
	if(accept_call_waiting != NULL)
	{
		accept_socket[0] = socket;
		incomplete_sockets.remove(socket);
		Continuation* accept_call = accept_call_waiting;
		accept_call_waiting = NULL;
		accept_call->success();
	}
	else                          
	{
		complete_sockets.push_back(socket);
		incomplete_sockets.remove(socket);
	} 
}

  /** Read nbytes data in to char buffer buf.
   *  Will invoke the caller.success() method after nbytes bytes of data
   *  have been received; or will invoke the caller.failure() method if 
   *  the TCP connection has been dropped.
   *  This is a simulated blocking method, and the read failure will be
   *  returned if an attempt is made to read while the previous read's
   *  Continuation has not yet returned.
   */
void TcpSocket::read(char* buf, int nbytes,  Continuation* caller)
{  
	app_call_waiting = caller;
	appCallWaiting = true;
	myTcpAgent->read(buf,nbytes,caller); 
}

  /** Write nbytes bytes of data. This will cause one or more TCP segments
   *  to be transmitted, with the total payload size equal to nbytes.
   *  Will invoke the caller.success() method after nbytes bytes of data
   *  have been acknowledged; or will invoke the caller.failure() method
   *  if the TCP connection has been dropped.
   *  This is a simulated blocking method, and the write failure will
   *  be returned if an attempt is made to write while the previous write's 
   *  Continuation has not yet returned.
   */
void TcpSocket::write(const char* const buf, int nbytes, Continuation* caller)
{  
	app_call_waiting = caller;
  appCallWaiting = true;
	myTcpAgent->advance_bytes(nbytes,buf,caller);   
}

  /** Close a socket and release all its resources.
   *  On return this socket object is no longer usable as a communications
   *  endpoint. If close a socket when a connection is established, initiate
   *  TCP disconnection procedure. If close a passively listening socket,
   *  this method first aborts any pending connections on its connection
   *  request queue, returns the Continuation failure for any pending accept()
   *  call, and finally returns caller.success().
   */
void TcpSocket::close(Continuation* caller)
{ 
  if (isListening) {
      while(!incomplete_sockets.empty()){
        TcpSocket* sock = (TcpSocket*)incomplete_sockets.front();
        sock->abort();
        incomplete_sockets.pop_front();
		    delete sock;
      }
      if (accept_call_waiting != NULL) {
        accept_call_waiting->failure(); 
        accept_call_waiting = NULL;
      }
      app_call_waiting = NULL;
      tcpMaster->removeServer(source_port);
      caller->success();
    } else {
      app_call_waiting = caller;
      appCallWaiting = true;
      myTcpAgent->close();
      socketState |= SS_ISDISCONNECTING;
    }
}                      

/**
 * Callback from tcpAgent when an established connection is disconnected.
 */
void TcpSocket::disconnected(){
    socketState ^= SS_ISDISCONNECTING;

    if (!appCallWaiting){
      socketInfo("no application is disconnecting!");
      return;
    }
    appCallWaiting = false;  
    app_call_waiting->success();
}


void TcpSocket::abort()
{   
  cerr << " Socket: connection aborted! " << endl;   
}

void TcpSocket::setTcpMaster(TcpMaster * tm) {
  tcpMaster = tm;
}





