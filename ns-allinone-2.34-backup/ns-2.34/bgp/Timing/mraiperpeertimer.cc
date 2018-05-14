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

#include "mraiperpeertimer.h"     
#include "../rtProtoBGP.h"
#include "../peer-entry.h"


  // ----- constructor MRAIPerPeerTimer ------------------------------------ //
  /**
   * Constructs a per-peer Minimum Route Advertisement Interval Timer with the
   * given parameters.
   *
   * @param b    The rtProtoBGP with which this timer is associated.
   * @param pr   The peer to whom this timer applies.
   */
MRAIPerPeerTimer::MRAIPerPeerTimer(rtProtoBGP *b,PeerEntry *pr)
  : TimerHandler(){
  bgp = b;
  peer = pr;
}

MRAIPerPeerTimer::~MRAIPerPeerTimer() {}


  // ----- MRAIPerPeerTimer.expire --------------------------------------- //
  /**
   * This method executes when the timer expires.  Updates are composed for any
   * prefixes that were waiting to be advertised or withdrawn, and a new timer
   * is started (if any new updates were in fact sent).
   */
void MRAIPerPeerTimer::expire(Event*)
{
    bool update_sent = false;
    
    if (Global::wrate) {
      map<unsigned int,IPaddress*>::iterator iter;
      for (iter = (peer->waiting_wds).begin();
           iter != (peer->waiting_wds).end(); iter++) {
        IPaddress * waitingwd = (*iter).second;
        // send the waiting withdrawal
        UpdateMessage * upmsg = new UpdateMessage(bgp->nh, waitingwd);
        bgp->send(upmsg, peer);
        update_sent = true;
        bgp->reset_timer(peer, BGP_Timer::KEEPALIVE);
      }
      // remove every element in the table
      peer->waiting_wds.clear(); 
    }

    map<unsigned int,AdvPair>::iterator iter;
    
    for (iter=peer->waiting_adv.begin(); iter!=peer->waiting_adv.end(); iter++) {
      AdvPair pair = iter->second;   
      Route * waitingrte = pair.rte;
      if ( waitingrte == NULL) {
        peer->waiting_adv.erase(iter);
        continue;
      }  
      
      // advertise the waiting route
      UpdateMessage * upmsg = new UpdateMessage(bgp->nh, waitingrte);
      bgp->send(upmsg, peer);
      update_sent = true;
      bgp->reset_timer(peer, BGP_Timer::KEEPALIVE);
      peer->waiting_adv.erase(iter);
    }
    // remove every element in the table
    peer->waiting_adv.clear(); 
     
    if (update_sent) {
      // start a new MRAIPerPeerTimer
      peer->mraiTimer = new MRAIPerPeerTimer(bgp,peer);
      peer->mraiTimer->resched((double)peer->mrai);

    } // else there is no waiting advertisement (or withdrawal)
      
}

  // ----- MRAIPerPeerTimer.reset --------------------------------------------- //
  /**
   * Force to reset the timer,
   */
void MRAIPerPeerTimer::reset() {
		if (status_ == TIMER_PENDING) {
			cancel();
    }
		status_ = TIMER_IDLE;
}
