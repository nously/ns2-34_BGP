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

#ifndef MRAIPERPEERTIMER_H
#define MRAIPERPEERTIMER_H

#include "timer-handler.h" 
#include "iostream"  // Changed by Reza Sahraei
#include "../route.h"
#include "../global.h"
#include "../Comm/updatemessage.h"

class rtProtoBGP;
class PeerEntry;
                                  
/**
 * BGP's Minimum Route Advertisement Interval Timer as used for per-peer rate
 * limiting.  (Class MRAITimer is used for per-peer, per-destination rate
 * limiting.)  This Minimum Route Advertisement Interval Timer has its own
 * implementation (instead of using the more generic TimerHandler class)
 * because there is a significant amount of specialized work that needs to
 * be done once it expires.  The expire method in this class encapsulates
 * that work.  The timer is to help ensure that peers do not receive update
 * messages too often.
 */
class MRAIPerPeerTimer : public TimerHandler  {
public:
	MRAIPerPeerTimer(rtProtoBGP *b,PeerEntry *pr);
	~MRAIPerPeerTimer();
  inline bool is_pending() { return status_ == TIMER_PENDING; }
  inline bool is_idle() { return 	status_ == TIMER_IDLE;}
  void reset();
  
protected:
  void expire(Event *e);
  
  /** The rtProtoBGP with which this timer is associated. */
  rtProtoBGP *bgp;

  /** The entry of the peer to whom a message was sent. */
  PeerEntry *peer;
  
};

#endif
