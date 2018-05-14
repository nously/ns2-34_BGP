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
 *          Hongbo Liu.
 */

#ifndef ns_tcp_master_h
#define ns_tcp_master_h

#include <tclcl.h>
#include "agent.h"  
#include "continuation.h" 
#include <list>
#include <iostream> // Changed by Reza Sahraei
#include "ip.h" 

using namespace std;      

class FullTcpAgent;
class TcpSocket;

typedef struct
{
	FullTcpAgent* agent;
	int src_ip,src_port;
	int des_ip,des_port;
}AgentInfo;
                  
class TcpMaster  : public Agent
{
public:
	TcpMaster();
	~TcpMaster();
	int command(int argc, const char*const* argv);
	FullTcpAgent* openServer(int src_port,TcpSocket* listening);
	FullTcpAgent* findServer(int src_port);
	void addServer(int src_port, FullTcpAgent* tcpAgent);
	FullTcpAgent* openConnect(int des_port,int des_ip,TcpSocket* ts);
	FullTcpAgent* findAgent(int des_ip, int des_port);
	void addAgent(int des_ip, int des_port, FullTcpAgent* tcpAgent);
	void addAgent(FullTcpAgent* tcpAgent);
  void removeAgent(int src_port);
  void removeReadTcpAgent(FullTcpAgent* tcpAgent);
	FullTcpAgent* newInComing(Packet* pkt, TcpSocket* listening);
	void removeServer(int src_port);
	void TcpMasterInfo(char* info);

private:
	const static int src_ip = 1;
	list<AgentInfo> agentsList;
  list<AgentInfo> serversList;
};

#endif
