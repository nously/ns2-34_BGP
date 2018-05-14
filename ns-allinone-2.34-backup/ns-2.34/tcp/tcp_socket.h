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

#ifndef ns_tcpsocket_h
#define ns_tcpsocket_h

#include "tcp-full.h"
#include "continuation.h"
#include "tcp_master.h"
#include <list>
#include "iostream" // Changed by Reza Sahraei
#include <string>   

#define SS_NBIO 1
#define SS_ISCONNECTED 2
#define SS_ISCONNECTING 4
#define SS_ISDISCONNECTING 8
#define SS_CANTRCVMORE 16

using namespace std;

/** BSD-style TCP socket pseudoprotocol for implementing
 *  protocols and applications that use TCP transport.
 */
class TcpSocket
{
friend class TcpMaster; 

public:
	TcpSocket();
	~TcpSocket();
	void bind(int src_ip, int src_port);
  void listen();
	void connect(int remote_ip, int remote_port, Continuation* caller);
	void connected();
	void accept(TcpSocket** newSocket, Continuation* caller);
	void read(char* buf, int nbytes,  Continuation* caller);
	void write(const char* const buf, int nbytes, Continuation* caller);
	void close(Continuation* caller);
  void disconnected();
	void socketInfo(char* info);
	void addSocket(TcpSocket* sock, int s_addr, int s_port);
	void addConnection(TcpSocket* socket);
	void abort();
	void bindTcpAgent(FullTcpAgent* agent);
  void unbindTcpAgent();
  void setTcpMaster(TcpMaster * tm);

private:

  /*********************   socket state   *********************/
  
  /** If the socket was created by a listening socket in
   *  response to a connection request, contains the reference
   *  to the listening socket. Used by tcpAgent to inform
   *  the listening socket about processing of the pending request.
   *  Used by an incomplete socket if connection was dropped
   *  to get removed from the incomplete socket queue by its
   *  listening socket.  */
	unsigned int socketState;
  
  // tcpAgent bound to this tcpSocket
	FullTcpAgent* myTcpAgent;
  
  // Cached references to Continuation's simulating the last
  // blocking calls in a blocking socket. Will be called back when
  // the condition causing the blocking is changed.
	Continuation* app_call_waiting;
  Continuation* connect_waiting;
	Continuation* accept_call_waiting;
	TcpMaster * tcpMaster;
  
  // connection request queues
	list<TcpSocket*> incomplete_sockets;
	list<TcpSocket*> complete_sockets;
	TcpSocket** accept_socket;

public:
	TcpSocket* listeningSocket;
  bool isListening;
  bool appCallWaiting;
	int source_ip_addr;
	int dest_ip_addr;
	int destination_port;
	int source_port;

};

#endif
