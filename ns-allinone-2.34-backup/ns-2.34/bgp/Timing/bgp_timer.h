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

#ifndef BGP_TIMER_H
#define BGP_TIMER_H

#include "iostream" // Changed by Reza Sahraei
#include "timer-handler.h"
#include "../Comm/startstopmessage.h"
#include "../Timing/timeoutmessage.h"

class rtProtoBGP;
class PeerEntry;

/**
 * Used to represent most of BGP's timers, with the exception of the Minimum
 * Route Advertisement Interval Timer, which has its own class (MRAITimer).
 */
class BGP_Timer : public TimerHandler  {   

public:    
	BGP_Timer(rtProtoBGP *b, int et, PeerEntry *pr);
  void expire(Event *e);
  void reset();
  inline bool is_pending() { return status_ == TIMER_PENDING; }
  inline bool is_idle() { return 	status_ == TIMER_IDLE;}
  
  // ......................... constants ........................... //
  /** Indicates the Connect Retry Timer. */
  const static int CONNRETRY  = 0;
  /** Indicates the Hold Timer. */
  const static int HOLD       = 1;
  /** Indicates the Keep Alive Timer. */
  const static int KEEPALIVE  = 2;
  /** Indicates the Minimum AS Origination Timer. */
  const static int MASO       = 3;
  /** Indicates the Minimum Route Advertisement Interval Timer. */
  const static int MRAI       = 4;
  
protected:
  /** The rtProtoBGP using this timer. */
  rtProtoBGP *bgp;
  
  /** The type of event associated with this timer.  Possible types are listed
   *  in class rtProtoBGP. */
  int event_type;
  
   /** The entry of the peer to whom this timer applies. */
  PeerEntry *peer;
};

#endif
