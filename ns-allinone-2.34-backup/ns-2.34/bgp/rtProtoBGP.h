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
 
#ifndef bgp_agent_h
#define bgp_agent_h

#include <stdio.h>
#include <iostream> // Changed by Reza Sahraei
#include <stdlib.h>
#include <vector>
#include <string>
#include <map>
#include <unistd.h>

#include "rng.h"
#include "node.h"
#include "global.h"
#include "locrib.h"
#include "peer-entry.h"
#include "tcp_socket.h"
#include "classifier-ipv4.h"


using namespace std;

class rtProtoBGP : public Agent {
public:
	rtProtoBGP();
	int  command(int argc, const char*const* argv);
  void cmd_network(const char * ipa);
  void cmd_no_network(const char * ipa);
  void show_routes();
  void conf();
  void reset_timer(PeerEntry * peer, int timertype);
  void ftadd(RouteInfo * info);
  void ftrmv(RouteInfo * info);
  void handle_update(UpdateMessage * msg);
  int  dop(Route * rte);
  void decision_process_1(vector<RouteInfo*> newinfo_list);
  vector<RouteInfo*> decision_process_2(vector<RouteInfo*> changedroutes);
  void decision_process_3(vector<RouteInfo*> locribchanges);
  void remove_all_routes(PeerEntry * peer);
  void external_update(map<PeerEntry*,vector<IPaddress*> > wds_tbl,
                       map<PeerEntry*,vector<RouteInfo*> > ads_tbl);
  bool advertisable(RouteInfo * info, PeerEntry * rcvr);
  void send(BGPMessage * msg, PeerEntry * peer);
  void try_send_update(UpdateMessage * msg, vector<string> senders,PeerEntry * peer);
  void listen();
  bool push(BGPMessage * message);
  bool receive(BGPMessage * message);
  inline float now() {return Scheduler::instance().clock();}
 
public:
  // ......................... constants ........................... //

  /** The developer's version string of this implementation of BGP-4. */
  const static string version;

  /** The well-known port number for BGP. */
  const static int PORT_NUM = 179;        
 
  // . . . . . . . . . . . connection states . . . . . . . . . . . //
  
  /** Indicates the Idle state in the BGP finite state machine (FSM). */
  const static int IDLE         = 1;
  /** Indicates the Connect state in the BGP finite state machine (FSM). */
  const static int CONNECT      = 2;
  /** Indicates the Active state in the BGP finite state machine (FSM). */
  const static int ACTIVE       = 3;
  /** Indicates the OpenSent state in the BGP finite state machine (FSM). */
  const static int OPENSENT     = 4;
  /** Indicates the OpenConfirm state in the BGP finite state machine (FSM). */
  const static int OPENCONFIRM  = 5;
  /** Indicates the Established state in the BGP finite state machine (FSM). */
  const static int ESTABLISHED  = 6;            
  
  // . . . . . . . . . . . event types . . . . . . . . . . . //
  
  /** Indicates an event that causes the BGP process to start up. */
  const static int BGPrun            =  0;
  /** Indicates the BGP Start event type. */
  const static int BGPstart          =  1;
  /** Indicates the BGP Stop event type. */
  const static int BGPstop           =  2;
  /** Indicates the BGP Transport Connection Open event type. */
  const static int TransConnOpen     =  3;
  /** Indicates the BGP Transport Connection Closed event type. */
  const static int TransConnClose    =  4;
  /** Indicates the BGP Transport Connection Open Failed event type. */
  const static int TransConnOpenFail =  5;
  /** Indicates the BGP Transport Fatal Error event type. */
  const static int TransFatalError   =  6;
  /** Indicates the ConnectRetry Timer Expired event type. */
  const static int ConnRetryTimerExp =  7;
  /** Indicates the Hold Timer Expired event type. */
  const static int HoldTimerExp      =  8;
  /** Indicates the KeepAlive Timer Expired event type. */
  const static int KeepAliveTimerExp =  9;
  /** Indicates the Receive Open Message event type. */
  const static int RecvOpen          = 10;
  /** Indicates the Receive KeepAlive Message event type. */
  const static int RecvKeepAlive     = 11;
  /** Indicates the Receive Update Message event type. */
  const static int RecvUpdate        = 12;
  /** Indicates the Receive Notification Message event type. */
  const static int RecvNotification  = 13;

  
  // ........................ member data .......................... //

  /** Whether or not the BGP process represented by this BGPSession object is
   *  actually alive.  If it is not (for example, if its router has crashed),
   *  then the protocol will not interact with anything else in the simulation
   *  until the process is restarted. */
  bool alive;  

  /** A socket listening for connection requests from (potential)
   *  peers (both internal and external). */
  TcpSocket * listensocket;

  /** The forwarding table, which supports IPv4 address and forwading schemes.
   *  It is the "live" table used for lookups when this router forwards packets
   *  (currently, IP is responsible for doing the forwarding).  */
  IPv4Classifier* fwd_table;

  /** The node which this BGP agent resides in */
  Node * node_;

  /** The String format of the AS number of this BGP speaker's AS. */
  string as_nh;

  /** The AS number of this BGP speaker's AS. */
  int as_num;

  /** The IP address prefix which is representative of this BGP's AS. */
  IPaddress * as_prefix;

  /** The NH part of the NHI address for this BGP's router. */
  string nh;

  /** The BGP Identifier for this BGP speaker.  Each BGP speaker (router
   *  running BGP) has a BGP Identifier.  A given BGP speaker sets the value of
   *  its BGP Identifier to an IP address assigned to that BGP speaker.  It is
   *  chosen at startup and never changes. */
  IPaddress * bgp_id;

  /** The integer value of bgp_id, needed for variable bindings. */
  int bgp_id_;

  /** The Loc-RIB.  It stores the local routing information that this BGP
   *  speaker has selected by applying its local policies to the routing
   *  information contained in the Adj-RIBs-In. */
  LocRIB * loc_rib; 

  /** Whether or not this instance of BGP serves as a route reflector. */
  bool reflector; 

  /** If this is a route reflector, the number of the cluster of which it is a
   *  member. */
  int cluster_num;

  /** A table of data for each neighboring router (potential BGP peer), keyed
   *  by the NH address prefix of that neighbor.  A router is considered a
   *  neighbor of the local router if there is a point-to-point connection
   *  between the two.  Every neighboring router ("neighbor" for short) is
   *  considered to be a potential peer at simulation start-up.  A peer is
   *  simply a neighbor with whom a BGP connection, or peering session, has
   *  been established.  Thus, a neighbor is not necessarily a peer, but a peer
   *  is always a neighbor.  This difference between neighbors and peers is
   *  important, and the terminology used here attempts to be consistent with
   *  these definitions. */
  map<string,PeerEntry*> nbs;

  /** A vector of PeerEntry pointer for each neighboring router
    * (potential BGP peer). */
  vector<PeerEntry*> nbs_; 

  /** A table of data for each neighboring router (potential BGP peer), keyed
   *  by the IP address of the interface on that router to which the local
   *  router has a point-to-point connection. */
  map<int,PeerEntry*> nbsip;

  /** A vector of IPaddress for each staticly configured route. */ 
  vector<IPaddress*> static_rtes;
  
  /** The amount of time that should elapse between attempts
   *  to establish a session with a particular peer. */
  int connretry_interval;
  
  /** The Minimum AS Origination Interval: the minimum amount of time
   *  that must elapse between successive advertisements of update
   *  messages that report changes within this BGP speaker's AS. */
  int masoi;

  /** Jitter factor for Keep Alive Interval. */
  double keep_alive_jitter;
  
  /** Jitter factor for Minimum AS Origination Interval. */
  double masoi_jitter;
  
  /** Jitter factor for Minimum Route Advertisement Interval. */
  double mrai_jitter; 

  /** Whether or not rate-limiting should be applied on a per-peer,
   *  per-destination basis.  The default is false: rate-limiting is applied
   *  only on a per-peer basis, without regard to destination. */
  bool rate_limit_by_dest;   

  /** The Minimum AS Origination Timer. */
  BGP_Timer * masoiTimer;

  /** A special peer entry which represents the local BGP speaker.  Obviously,
   *  this entry does not actually represent a peer at all, but it is useful
   *  when dealing with routes which were originated by this BGP speaker or
   *  configured statically. */
  PeerEntry * self;

  /** The TCP agent master. It manages all the TCP agents used by BGP,
    * such as creating a new TCP agent and closing an existing TCP agent.  */
  TcpMaster * tcpmaster;

  /** Whether this instance of BGP should be auto-configured.  If true, this 
   *  BGP instance will ignore any TCL configuration attributes and use default
   *  rules to set various values and for establishing peering sessions.  The
   *  default value is false, which is set in tcl/lib/ns-default.tcl. */
  int auto_config;                   
 
};
#endif
