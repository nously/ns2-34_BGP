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

#include "mraitimer.h"
#include "../rtProtoBGP.h"
#include "../peer-entry.h"  

  // ----- constructor MRAITimer ------------------------------------------- //
  /**
   * Constructs a Minimum Route Advertisement Interval Timer with the given
   * parameters.
   *
   * @param b    The BGPSession with which this timer is associated.
   * @param ipa  The NLRI from the update message which resulted in the
   *             need for this timer to be set.
   * @param pr   The peer to whom this timer applies.
   */
MRAITimer::MRAITimer(rtProtoBGP *b,IPaddress* ipa,PeerEntry *pr)
          : TimerHandler() {
  bgp = b;
  nlri = ipa;
  peer = pr;  
}    

MRAITimer::~MRAITimer(){
}

  // ----- MRAITimer.expire ---------------------------------------------- //
  /**
   * When the timer expires, this method removes the IP address from the list
   * of recently sent updates, sends an update with the advertisement (or
   * possibly withdrawal, if the option to apply MRAI to withdrawals is in use)
   * that was waiting to be sent (if there is one), and restarts a new timer
   * (if a waiting advertisement (or withdrawal) was in fact sent).
   */
void MRAITimer::expire(Event*)
{
    IPaddress * adv_nlri = peer->adv_nlri[nlri->masked_val()];
    peer->adv_nlri.erase(nlri->val());
    IPaddress * wdn_nlri = NULL;
    
    if (Global::wrate) {
      wdn_nlri = peer->wdn_nlri[nlri->masked_val()];
      peer->wdn_nlri.erase(nlri->val());
    }
    
    AdvPair pair = peer->waiting_adv[nlri->masked_val()];
    peer->waiting_adv.erase(nlri->val());
    Route * waitingrte = NULL;
    
    if (pair.rte != NULL) {
      waitingrte = pair.rte;
    }

    IPaddress * waitingwd = NULL;
    if (Global::wrate) {
      waitingwd = peer->waiting_wds[nlri->masked_val()];
      peer->waiting_wds.erase(nlri->val());
    }

    if (waitingrte != NULL) {
      if (waitingwd != NULL) {
         cerr<< "unexpected waiting withdrawal" << endl;
      }
      // advertise the waiting route
      UpdateMessage * upmsg = new UpdateMessage(bgp->nh, waitingrte);
      bgp->send(upmsg, peer);
      bgp->reset_timer(peer, BGP_Timer::KEEPALIVE);   
      // start a new MRAITimer
      MRAITimer * newtimer = new MRAITimer(bgp, nlri, peer);
      newtimer->resched((double)peer->mrai);
      peer->mrais[nlri->masked_val()] = newtimer;
      // and since we just advertised a route, add it to the adv_nlri table
      peer->adv_nlri[nlri->masked_val()] = nlri;

    } else if (waitingwd != NULL) {
      // send the waiting withdrawal
      UpdateMessage * upmsg = new UpdateMessage(bgp->nh, waitingwd);

      bgp->send(upmsg, peer); 
      bgp->reset_timer(peer, BGP_Timer::KEEPALIVE);

      // start a new MRAITimer
      MRAITimer * newtimer = new MRAITimer(bgp, nlri, peer);
      newtimer->resched((double)peer->mrai);
      peer->mrais[nlri->masked_val()] = newtimer;
      // and since we just sent as withdrawal, add the NLRI to the wdn_nlri
      // table
      peer->wdn_nlri[nlri->masked_val()] = nlri;
    } // else there is no waiting advertisement (or withdrawal)         
}

void MRAITimer::reset() {
		if (status_ == TIMER_PENDING) {
			cancel();
    }
		status_ = TIMER_IDLE;
}