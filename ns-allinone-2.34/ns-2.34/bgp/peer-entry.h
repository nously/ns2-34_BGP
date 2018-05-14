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


#ifndef peer_entry_h
#define peer_entry_h

#include <stdio.h>
#include <iostream> // Changed by Reza Sahraei
#include <stdlib.h>
#include <tclcl.h>
#include <string>
#include <map>
#include <queue>
#include "agent.h"  
#include "global.h"
#include "Util/ipaddress.h"
#include "Util/radixtree.h"
#include "Timing/bgp_timer.h"
#include "Timing/mraiperpeertimer.h"
#include "Timing/mraitimer.h"
#include "Comm/transportmessage.h" 
#include "Comm/notificationmessage.h"
#include "Comm/keepalivemessage.h" 
#include "Comm/updatemessage.h"
#include "Comm/openmessage.h"
#include "Comm/startstopmessage.h"
#include "tcp_master.h"
#include "tcp_socket.h"
#include "adjribin.h"
#include "adjribout.h"

using namespace std;

struct AdvPair {
	Route * rte;
	string sender_nh;
  AdvPair() {
    rte = NULL;
    sender_nh = "";
  }
  bool operator==(const AdvPair& i) const {
        return (rte->equals(i.rte) && sender_nh == i.sender_nh);
  }  
};               

class rtProtoBGP;

class PeerEntry : public Agent {
friend class Continuation;
public:
	PeerEntry();
	virtual int command(int argc, const char*const* argv);
  inline string addr() { return ip_addr->toString(); }
  bool equals(TclObject *pe);
  void connect();
  void receive();
  void send(BGPMessage *msg);
  void write(BGPMessage *msg);
  void write(BGPMessage *msg, TcpSocket *writesock);
  void close();
  void write_close(TcpSocket *wsocket);
  void read_close(TcpSocket *rsocket);
  void cancel_timers();  
      
public:
  // ......................... constants ........................... //
  
  /** Indicates an Internal BGP peer (in the same AS). */
  const static int INTERNAL =  1;
  /** Indicates an External BGP peer (in a different AS). */
  const static int EXTERNAL =  2;
  /** Indicates that the peer sub-type value is not applicable. */
  const static int NA        = 0;
  /** Indicates a route reflector client peer sub-type of internal peer. */
  const static int CLIENT    = 1;
  /** Indicates a route reflector non-client peer sub-type of internal peer. */
  const static int NONCLIENT = 2;
  
  // ........................ member data .......................... //

  /** The rtProtoBGP to which this peer information applies. */
  rtProtoBGP * bgp;

  /** The type of peer.  (Internal or external.) */
  int typ;

  /** The sub-type of the peer.  (Internal reflector client or non-client.) */
  int subtyp;

  /** The NH part of the NHI address of this peer.
   *  In this implementation, nh = AS_number:IP_address.
   */
  string nh;

  /** The local IP address that this peer uses as a destination when sending
   *  packets here.  For internal peers, this is typically the address of a
   *  virtual (loopback) interface, and for external peers it is typically the
   *  address of a physical interface on a point-to-point link directly
   *  connecting the two BGP speakers. */
  IPaddress * return_ip;
  
  /** The integer value of return_ip, needed for variable bindings. */
  int return_ipaddr_;

  /** The IP address of the interface on the peer's router which is
   *  linked to an interface on the local router. */
  IPaddress * ip_addr;

  /** The integer value of ip_addr, needed for variable bindings. */   
  int ip_addr_;

  /** The BGP ID of this peer. */
  IPaddress * bgp_id;

  /** The integer value of bgp_id, needed for variable bindings. */
  int bgp_id_;

  /** String format of the number of the AS in which this peer resides. */
  string as_nh;

  /** The number of the AS in which this peer resides. */
	int as_num;

  /** The state that the local BGP speaker is in for this peer. */
  int connection_state;

  /** The socket for receiving messages from this peer. */
  TcpSocket * readsocket;

  /** The socket for sending message to this peer. */
  TcpSocket * writesocket;                

  /** Maps read sockets to null/non-null (false/true).  This boolean value
   *  indicates whether or not the socket used for receiving messages from this
   *  peer is busy.  A busy socket means that a read is currently in
   *  progress. */
  map<TcpSocket*,bool> reading;

  /** Maps write sockets to null/non-null (false/true).  This boolean value
   *  indicates whether or not the socket used for sending messages to this
   *  peer is busy.  A busy socket means that a write is currently in
   *  progress. */
  map<TcpSocket*,bool> writing;

  /** Maps write sockets to null/non-null (false/true).  This boolean value, if
   *  true, indicates that either the outgoing socket connection has been
   *  established, or there is an attempt currently underway to establish it.
   *  The two cases can be distinguished by the corresponding value in the
   *  'writeconnected' mapping. */
  map<TcpSocket*,bool> writeconnecting;

  /** Maps write sockets to null/non-null (false/true).  This boolean value
   *  indicates whether or not an outgoing socket connection has been
   *  established. */
  map<TcpSocket*,bool> writeconnected;

  /** A queue of writes waiting to be performed on the write socket.  The queue
   * is necessary since a write may be issued before the previous write has
   * successfully completed.  (For a write to successfully complete, the
   * underlying TCP session must receive an acknowledgement for the bytes that
   * were sent.) */
  queue<BGPMessage*> writeq;

  /** The maximum amount of time (in clock ticks) which can elapse without any
   *  BGP message being received from this peer before the connection is
   *  broken. */
  int hold_timer_interval;

  /** The maximum amount of time (in clock ticks) which can elapse between
   *  messages sent to this peer (or else there's a risk that the peer could
   *  break the connection). */
  int keep_alive_interval; 

  /** The Minimum Route Advertisement Interval.  It is the minimum amount of
   *  time (in clock ticks) which must elapse between the transmission of any
   *  two advertisements containing the same destination (NLRI) to this
   *  peer.  This is the value of MRAI after jitter has been applied. */
  int mrai;

  /** The ratio between the configured values of the Keep Alive and Hold Timer
   *  Intervals. */
  double keephold_ratio;
 
  /** The ConnectRetry Timer, for spacing out attempts to establish a
   *  connection with this peer.  (The terminology used here is not quite
   *  correct--technically, it's not a peer until the connection is
   *  established, only a <i>potential</i> peer.) */ 
  BGP_Timer * crt;

  /** The Hold Timer, for timing out connections with peers. */
  BGP_Timer * ht;

  /** The KeepAlive Timer, for helping to ensure this peer doesn't time out its
   *  connection with us. */
  BGP_Timer * ka;

  /** A table of Minimum Route Advertisement Interval Timers.  These timers
   *  help ensure that this peer isn't flooded with several updates regarding
   *  the same destination (NLRI) in a short time.  It does not apply to
   *  internal peers.  The table is keyed by the NLRI. */
  map<unsigned int, MRAITimer*> mrais;

  /** The Minimum Route Advertisement Interval Timer used when per-peer
   *  rate-limiting only (no per-destination) is in use. */
  MRAIPerPeerTimer * mraiTimer;

  /** A table of NLRI recently advertised to this peer, used only when rate
   *  limiting is done on a per-peer, per-destination basis.  It is kept
   *  because BGP has limits on the number of times that the same NLRI can be
   *  sent within a given period of time to a given external peer.  The NLRI is
   *  used as both key and value in the table. */
  map<unsigned int,IPaddress*> adv_nlri;

  /** A table of NLRI recently withdrawn from this peer, used only when rate
   *  limiting is done on a per-peer, per-destination basis.  It is analogous
   *  to adv_nlri, to be used when the Minimum Route Advertisement
   *  restriction is being applied to withdrawals.  The NLRI is used as both
   *  key and value in the table. */
  map<unsigned int,IPaddress*> wdn_nlri;

  /** A table of prefixes that are waiting to be advertised to this peer.  More
   *  specifically, each entry is keyed by such a prefix, but the value is a
   *  pair of objects indicating the route and the sender of the advertisement
   *  for the route.  Prefixes can be waiting to be sent either because 1) an
   *  update with the same NLRI was sent recently (if per-peer, per-destination
   *  rate limiting is in use) or 2) an update with any prefix was sent
   *  recently (if per-peer rate limiting is in use). */
  map<unsigned int,AdvPair> waiting_adv;

  /** A table of prefixes which are waiting to be withdrawn from this peer.
   *  This is similar to the waiting_adv field, and is only used
   *  when the option to apply the Minimum Route Advertisement Interval
   *  restriction to withdrawals is in use.  The prefix is used as both key and
   *  value in the table. */
  map<unsigned int,IPaddress*> waiting_wds;

  /** The section of Adj-RIBs-In associated with this peer. */
  AdjRIBIn * rib_in;

  /** The section of Adj-RIBs-Out associated with this peer. */
  AdjRIBOut * rib_out;

  /** The number of updates received from this peer during the current
   *  session. */
  int inupdates;

  /** The number of updates sent to this peer during the current session. */
  int outupdates;

  /** Whether or not a connection with this (potential) peer has been
   *  established yet. */
  bool connected;
  
};

#endif

