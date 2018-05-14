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

#include "tcp_master.h"
#include "tcp-full.h"
#include "tcp_socket.h"

/**
 * This class manages all the TCP agents used by BGP, such as creating a new
 * TCP agent and closing an existing TCP agent.
 */
static class TcpMasterclass : public TclClass {
  
public:
	TcpMasterclass() : TclClass("Agent/TcpMaster") {}
	TclObject* create(int, const char*const*) {
		return (new TcpMaster);
	}
} class_tcp_master;  

TcpMaster::TcpMaster(): Agent(PT_TCPMASTER) { 
}   

TcpMaster::~TcpMaster()	{
}


int TcpMaster::command(int argc, const char*const* argv)
{  
  return Agent::command(argc, argv); 
}   

 /**
  * Passive open of a listening server.
  */  
FullTcpAgent* TcpMaster::openServer(int s_port,TcpSocket* listening)
	{
    if(findServer(s_port))
		{
			TcpMasterInfo("The server already existed!");
			return NULL;
		} 
    Tcl& tcl = Tcl::instance();
		tcl.evalf("%s Open-Server %d",name(),s_port);
		FullTcpAgent* agent = (FullTcpAgent*) TclObject::lookup(tcl.result());
    listening->bindTcpAgent(agent);
    agent->tcpMaster = this;
    agent->mySocket = listening;
		agent->listen();
  	addServer(s_port,agent);
    return agent;
}

  /**
   * Actively open a connection, creating a writing tcpAgent
   */
FullTcpAgent* TcpMaster::openConnect(int des_port,int des_ip,TcpSocket* writing)
	{
     FullTcpAgent * agent = findAgent(des_ip,des_port);
     if( agent != NULL)
		  {
        agent->listen();
			}  else {      
        Tcl& tcl = Tcl::instance();
		    tcl.evalf("%s New-OutGoing %d %d",name(),des_port,des_ip);
		    agent = (FullTcpAgent*) TclObject::lookup(tcl.result());
      }
      writing->bindTcpAgent(agent);
      agent->tcpMaster = this;
      agent->mySocket = writing;    
    
      // calling connect() through advance-bytes()
      agent->advance_bytes(0);
		  addAgent(des_ip,des_port,agent);    
      return agent;
	}

  /**
   * Create a reading tcpAgent in response to received connection request.
   */
FullTcpAgent* TcpMaster::newInComing(Packet* pkt, TcpSocket* listening)
	{
   	Tcl& tcl = Tcl::instance();
    hdr_ip* iph = hdr_ip::access(pkt);
    FullTcpAgent* agent = findAgent(iph->saddr(),iph->sport());
    if( agent != NULL)
		  {
        delete agent;
        agent = NULL;

			}
   	tcl.evalf("%s New-InComing %d %d %d",name(),iph->sport(),
                                         iph->saddr(),iph->dport());
		agent = (FullTcpAgent*) TclObject::lookup(tcl.result());
    agent->tcpMaster = this;
		TcpSocket* sock = new TcpSocket();
    listening->addSocket(sock,iph->saddr(),iph->sport());
 		sock->bind(src_ip,listening->myTcpAgent->port());
 	  sock->bindTcpAgent(agent);
    sock->setTcpMaster((TcpMaster*)this);
    agent->mySocket = sock;
		addAgent(agent);
        
		agent->listen();
		agent->recv(pkt,NULL);
    return agent;
	}


  /**
   * Add a tcp agent to the TCP agent list by its identification
   * via destination IP address and port number.
   */  
void TcpMaster::addAgent(int des_ip,int des_port,FullTcpAgent* tcpAgent)
	{
		AgentInfo agentInfo;
		agentInfo.agent = tcpAgent;
		agentInfo.des_ip = des_ip;

		agentInfo.des_port = des_port;
		agentsList.push_back(agentInfo);
	}

  /**
   * Add a tcp agent to the TCP agent list by given the agent
   */
void TcpMaster::addAgent(FullTcpAgent* tcpAgent)
	{
		AgentInfo agentInfo;
		agentInfo.agent = tcpAgent;
		agentsList.push_back(agentInfo);
	}

  /**
   * Find a tcp agent in the TCP agent list by its identification
   * via destination IP address and port number.
   */
FullTcpAgent* TcpMaster::findAgent(int des_ip,int des_port)
	{
		list<AgentInfo>::iterator theIterator;
		for(theIterator=agentsList.begin();theIterator!=agentsList.end();theIterator++)
		{
			if((*theIterator).des_ip == des_ip
				&& (*theIterator).des_port == des_port)
				return (*theIterator).agent;
		}
		return NULL;
	}

  /**
   * Add a tcp server to the TCP server list 
   * via the src_port it listens to.
   */   
void TcpMaster::addServer(int src_port,FullTcpAgent* tcpAgent)
	{
		AgentInfo agentInfo;
		agentInfo.agent = tcpAgent;
		agentInfo.src_port = src_port;
		serversList.push_back(agentInfo); 
	}

  /**
   * Find the listening (server) tcpAgentbound to the given local port number. 
   * @return tcp agent representing this server if the server exist,
   * otherwise return null.  
   */
FullTcpAgent* TcpMaster::findServer(int src_port)
	{
		list<AgentInfo>::iterator theIterator;


 
		for(theIterator=serversList.begin();theIterator!=serversList.end();theIterator++)
		{
			if((*theIterator).src_port == src_port) {
				return (*theIterator).agent;
      }  
		}
    return NULL;
    
  }

void TcpMaster::TcpMasterInfo(char* info)
  {
		printf("%s\n",info);
  }

 /**
  * Remove a pasive server session listening on local port SrcPort
  */
void TcpMaster::removeServer(int src_port) 
  {
		list<AgentInfo>::iterator theIterator;
		for(theIterator=serversList.begin(); theIterator!=serversList.end();theIterator++)
		{
			if((*theIterator).src_port == src_port)
			{
				serversList.erase(theIterator);
				return;
			}
		}
		TcpMasterInfo("Cannot find the server to remove!");
  }

 /**
  * Remove a tcpAgent
  */  
void TcpMaster::removeAgent(int src_port)

	{
		list<AgentInfo>::iterator theIterator;
		for(theIterator=agentsList.begin(); theIterator!=agentsList.end();theIterator++)
		{
			if((*theIterator).src_port == src_port)
			{
				agentsList.erase(theIterator);
				return;
			}
		}
		TcpMasterInfo("Cannot find the agent to remove!");
	}   
 
  /**
   *  Remove the reading tcpAgent when the peer entry using it successfully   
   *  read_closed.
   */  
void TcpMaster::removeReadTcpAgent(FullTcpAgent* agent)
  {
    Tcl& tcl = Tcl::instance();    
		tcl.evalf("%s Remove-InComing %d %d ",name(),agent->daddr(), agent->port()); 
  } 