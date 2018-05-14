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

#include "bgp_timer.h"  
#include "../rtProtoBGP.h"
#include "../peer-entry.h"      


  // ----- constructor BGP_Timer(rtProtoBGP,int,PeerEntry) ------------ //
  /**
   * A basic constructor to initialize member data.  For convenience, it takes
   * an integer instead of a long.
   *
   * @param b   The BGPSession with which this timer is associated.
   * @param et  The type of timeout associated with this timer.
   * @param pr  The entry of the peer to whom this timer applies.
   * 
   */
BGP_Timer::BGP_Timer(rtProtoBGP *b,int et,PeerEntry *pr): TimerHandler(){
  bgp = b;
  event_type = et;
  peer = pr;
}    

  // ----- BGP_Timer.expire --------------------------------------------- //
  /**
   * Sends a timeout message to the owning rtProtoBGP when the timer expires.
   */
void BGP_Timer::expire(Event*)
{
    // essentially, BGP is calling push() on itself
	  if (event_type==rtProtoBGP::BGPstart || event_type==rtProtoBGP::BGPstop) {
      bgp->push(new StartStopMessage(event_type, peer->nh));
    } else if (event_type==rtProtoBGP::BGPrun) {
      bgp->push(new BGPMessage(BGPMessage::RUN, peer->nh));
    } else {
     bgp->push(new TimeoutMessage(event_type, peer->nh));
    }     
}

  // ----- BGP_Timer.reset --------------------------------------------- //
  /**
   * Force to reset the timer, 
   */
void BGP_Timer::reset() {
		if (status_ == TIMER_PENDING) {
			cancel();
    }  
		status_ = TIMER_IDLE;
		     
}  


