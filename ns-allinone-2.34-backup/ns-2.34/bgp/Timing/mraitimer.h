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

#ifndef MRAITIMER_H
#define MRAITIMER_H

#include "iostream" // Changed by Reza Sahraei
#include "timer-handler.h"
#include "../../common/scheduler.h"
#include "../Util/ipaddress.h"

class rtProtoBGP;
class PeerEntry;

/**
 * BGP's Minimum Route Advertisement Interval Timer.  The Minimum Route
 * Advertisement Interval Timer has its own implementation (instead of using
 * the more generic TimerHandler class) because there is a
 * significant amount of specialized work that needs to be done once it
 * expires.  The expire method in this class encapsulates that work.  The
 * timer is to help ensure that peers do not receive update messages with
 * routes regarding the same destination too often.
 */
class MRAITimer : public TimerHandler  {
public:
	MRAITimer(rtProtoBGP *b,IPaddress* ipa,PeerEntry *pr);
	~MRAITimer();
  inline bool is_pending() { return status_ == TIMER_PENDING; }
  inline bool is_idle() { return 	status_ == TIMER_IDLE;}
  void reset();
  
protected:
  void expire(Event *e);
  
  /** The rtProtoBGP with which this timer is associated. */
  rtProtoBGP *bgp;
  
  /** The NLRI from the update message which caused this timer to start. */
  IPaddress * nlri;
  
  /** The entry of the peer to whom a message was sent. */
  PeerEntry *peer;
};

#endif
