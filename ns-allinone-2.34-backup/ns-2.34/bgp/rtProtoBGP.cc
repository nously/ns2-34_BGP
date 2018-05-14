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

#include "rtProtoBGP.h"
        
char * statestr[7] = { "", "Idle", "Connect", "Active","OpenSent", "OpenConfirm","Established" };
char * eventNames[14] = { "BGPrun", "BGPstart", "BGPstop",
     "TransConnOpen", "TransConnClose", "TransConnOpenFail", "TransFatalError",
     "ConnRetryTimerExp", "HoldTimerExp", "KeepAliveTimerExp", "RecvOpen",
     "RecvKeepAlive", "RecvUpdate", "RecvNotification" };

static class rtProtoBGPclass : public TclClass {
public:
	rtProtoBGPclass() : TclClass("Agent/rtProto/BGP") {}
	TclObject* create(int, const char*const*) {
		return (new rtProtoBGP);
	}
} class_rtProtoBGP;         

  // ----- constructor rtProtoBGP ------------------------------------------ //
  /**
   * Constructs a BGP protocol agent.
   */
rtProtoBGP::rtProtoBGP() : Agent(PT_RTPROTO_BGP){

		bind ("connretry_interval_",&connretry_interval);
		bind ("masoi_",&masoi);
    bind ("bgp_id_",&bgp_id_);
    bind ("as_num_",&as_num);
    bind ("cluster_num",&cluster_num);
    bind_bool("auto_config_",&auto_config);

    reflector = false;          
    alive = false;
    rate_limit_by_dest = Global::rate_limit_by_dest;    
}   



int rtProtoBGP::command(int argc, const char*const* argv)
{
	Tcl & tcl = Tcl::instance();
 	if (argc == 2) {
    if (strcmp(argv[1], "set-reflector") == 0) {
       reflector = true;
		   return TCL_OK;
	  }
    if (strcmp(argv[1], "show-routes") == 0) {
      show_routes();
		  return TCL_OK;
    }
	  if (strcmp(argv[1], "conf") == 0) {
      conf();
		  return TCL_OK;
	  }
  }
  else if (argc == 3) {  
	  if (strcmp(argv[1], "new-peer") == 0) {    
		  PeerEntry * peer = (PeerEntry*) TclObject::lookup(argv[2]);
		  nbs_.push_back(peer);
		  return TCL_OK;
	  }  
    if (strcmp(argv[1], "bind-tcp-master") == 0) {
		  tcpmaster = (TcpMaster*) TclObject::lookup(argv[2]);
	    return TCL_OK;
    }
    if (strcmp(argv[1], "bind-classifier") == 0) {
		  fwd_table = (IPv4Classifier*) TclObject::lookup(argv[2]);
	    return TCL_OK;
    }
    if (strcmp(argv[1], "bind-node") == 0) {
		  node_ = (Node*) TclObject::lookup(argv[2]);
	    return TCL_OK;
    }
 	  if (strcmp(argv[1], "network") == 0) {
      cmd_network(argv[2]);
		  return TCL_OK;
	  }
    if (strcmp(argv[1], "no-network") == 0) {
      cmd_no_network(argv[2]);
		  return TCL_OK;
	  }
  }  
	return Agent::command(argc, argv);
}


  // ----- rtProtoBGP::show_routes ------------------------------------------ //
  /**
   * Display all routes in the local LIB.
   *
   */      
void rtProtoBGP::show_routes() {
  cout << "BGP routing table of n" << node_->nodeid() << endl; 
  cout << loc_rib->toString() << endl;
}

  // ----- rtProtoBGP::cmd_network ------------------------------------------ //
  /**
   *  The network command specifies the networks that the AS originates. It casues
   *  the BGP router to advertise the specified networks. 
   *
   * @param ipa    The the IP prefix that is being advertised.
   */                      
void rtProtoBGP::cmd_network(const char * ipa) {
   IPaddress * nlri = new IPaddress(string(ipa));
   if(!alive) {
     static_rtes.push_back(nlri);
   } else {
     // just add this rte to locrib
        Route * rte = new Route();
        rte->set_nlri(nlri);
        rte->set_origin(Origin::IGP);
        rte->set_orig_id(bgp_id);
        rte->set_nexthop(bgp_id);
        RouteInfo * info = new RouteInfo(this,rte,RouteInfo::MAX_DOP,
                                             true,self);
        loc_rib->replace(info);   // add info, replace old one, if there is any
        if (Global::auto_advertise) {
          // By inserting a route to this AS in the Loc-RIB and then starting
          // Phase 3 of the Decision Process, we effectively cause update
          // messages to be sent to each of our peers.  Note that we insert
          // into the Loc-RIB but *not* into the local router's forwarding
          // table.
          // run Phase 3 of the Decision Process so that the changes to the
          // Loc-RIB will get propagated to the Adj-RIBs-Out.
          vector<RouteInfo*> locribchanges; 
          locribchanges.push_back(info);
          decision_process_3(locribchanges);
        }
   }
}                                          

  // ----- rtProtoBGP::cmd_no_network ------------------------------------ //
  /**
   *  The no-network command invalidates the networks that the AS originates. 
   *  It casues the BGP router to withdraw the specified networks.
   *
   * @param ipa    The the IP prefix that is being invalidated.
   */                                                               
void rtProtoBGP::cmd_no_network(const char * ipa) {
    IPaddress * nlri = new IPaddress(string(ipa));
    // we assume no-network command only call when bgp is alive,
    // just remove this rte from locrib
    RouteInfo * info = loc_rib->remove(nlri);   
    if (Global::auto_advertise) {
          // run Phase 3 of the Decision Process so that the changes to the
          // Loc-RIB will get propagated to the Adj-RIBs-Out.              
          vector<RouteInfo*> locribchanges; 
          locribchanges.push_back(info);
          decision_process_3(locribchanges);
    }
}

  // ----- rtProtoBGP::conf ----------------------------------------------- //
  /**
   * Sets configurable values for BGP.  If the 'autoconfig' attribute is
   * set (in the TCL script file), then eBGP sessions will be created for
   * all neighbouring BGP routers.
   */                                            
void rtProtoBGP::conf()   {

    // - - - - - - - - set jitter factors - - - - - - - - //
    // jitter factors may vary between 0.75 and 1.00
    RNG rng1(Global::jitter_factor_seed);
    if (Global::jitter_masoi) {
      masoi_jitter = 0.75 + rng1.next_double()/4.0;
    } else {
      masoi_jitter = 1.0;
    }
    if (Global::jitter_keepalive) {
      keep_alive_jitter = 0.75 + rng1.next_double()/4.0;
    } else {
      keep_alive_jitter = 1.0;
    }
    if (Global::jitter_mrai) {
      mrai_jitter = 0.75 + rng1.next_double()/4.0;
    } else {
      mrai_jitter = 1.0;
    }

    // - - - - - - - create peer entries if auto_config is set - - - - - - - //
    // in which case, there is no tcl setup for eBGP peers .
    Tcl & tcl = Tcl::instance();
    if (auto_config) {                  
      as_num = node_->as_number();
      bgp_id_ = node_->address();
      nbs_.clear();       // just to be save.

      list<Node*>::iterator i;
      for(i=node_->ASnbs.begin();i!=node_->ASnbs.end();i++) {
        // ASnbs contains all peers which are connected by duplux-links to
        // this BGP router.
        tcl.evalf("%s neighbor %d remote-as %d",name(),
                     (*i)->address(),(*i)->as_number());
      }
    }

    // - - - - - - initialize the as_nh, bgp_id and cluster_num - - - - - - - //
    char * tempstring = new char[8];
    sprintf(tempstring,"%d",as_num);
    as_nh = tempstring; 

    bgp_id = new IPaddress((unsigned int)bgp_id_);
    if (cluster_num == 0) {  // not configured
      cluster_num =  (unsigned int)bgp_id_;
    } 

    // - - - - - - initialize a special peer entry: self - - - - - - - - //
    tcl.evalf("%s new-peer %d %d",name(),bgp_id_,bgp_id_); 
    self = (PeerEntry*) TclObject::lookup(tcl.result());
    self->bgp = this;
    self->ip_addr = new IPaddress((unsigned int)bgp_id_);
    self->return_ip = new IPaddress((unsigned int)bgp_id_);
    self->as_nh = as_nh;
    self->nh = self->as_nh +":"+ self->ip_addr->toString();     

    loc_rib = new LocRIB(this);

    // - - - - - - initialize a new set of peer data - - - - - - - - //
    for (int i=0;i<nbs_.size();i++){           
      PeerEntry * nb = nbs_[i];
      
      nb->bgp = this;
      nb->ip_addr = new IPaddress((unsigned int)nb->ip_addr_);
      nb->return_ip = new IPaddress((unsigned int)nb->return_ipaddr_); 
      nb->bgp_id = NULL;    // won't know until we hear from them    
                                                                                      
      sprintf(tempstring,"%d",nb->as_num);
      nb->as_nh = tempstring;
      nb->nh = nb->as_nh +":"+ nb->ip_addr->toString();
      
      nbs[nb->nh] = nb;
      nbsip[nb->ip_addr_] = nb;
      
      nb->rib_in  = new AdjRIBIn(nb,this);
      nb->rib_out = new AdjRIBOut(nb,this);
      
      nb->connection_state = IDLE;
        
      // NOTE: The value of the Keep Alive Timer Interval may change
      // during the peering session establishment process.
      nb->keephold_ratio = (double)nb->keep_alive_interval /
                             (double)nb->hold_timer_interval;
      nb->ka = new BGP_Timer(this, KeepAliveTimerExp, nb);
      nb->ka->resched(nb->keep_alive_interval);
      nb->ht = NULL;
      nb->crt = NULL;
      
      // - - - - - - - - set the MRAI timer - - - - - - - - - - - //
      nb->mrai = (int)(mrai_jitter * nb->mrai);
      if (!rate_limit_by_dest) {
        nb->mraiTimer = new MRAIPerPeerTimer(this, nb);
      }       
    }
    
    // - - - - - - - - set masoi value  - - - - - - - - - - - -//
    // This implementation doesn't actually use this timer (yet).
    masoi = (int)(masoi_jitter * masoi);
 
    // Start the timer ticking.  (It will "bring up" BGP when it goes off.)
    BGP_Timer * st = new BGP_Timer(this, BGPrun, self);
    st->resched(Global::base_startup_wait);                    
}
                                                                

  // ----- rtProtoBGP::reset_timer ------------------------------------------ //
  /**
   * Resets the indicated type of timer for the given peer (if applicable).  If
   * the timer had not been previously set, then the cancel has no effect, but
   * the timer is still set normally.
   *
   * @param peer   The peer entry for the peer with whom the timer
   *               is associated (if applicable).
   * @param timertype  The timer to be reset.
   */
void rtProtoBGP::reset_timer(PeerEntry * peer, int timertype) {
    switch (timertype) {
    case BGP_Timer::CONNRETRY:
      if (peer->crt != NULL) {
        peer->crt->reset();
      } else {
        peer->crt = new BGP_Timer(this, ConnRetryTimerExp, peer);
      }
      peer->crt->resched((double) connretry_interval);
      break;
    case BGP_Timer::HOLD:
      // if the negotiated Hold Timer interval is 0, then we don't
      // bother with the Hold Timer or the KeepAlive timer
      if (peer->hold_timer_interval > 0) {
        if (peer->ht != NULL) {  
          peer->ht->reset();
        } else {
          peer->ht = new BGP_Timer(this,HoldTimerExp, peer);
        }
        peer->ht->resched(peer->hold_timer_interval);
      }
      break;
    case BGP_Timer::KEEPALIVE:
      // if the negotiated Hold Timer interval is 0, then we don't
      // bother with the Hold Timer or the KeepAlive timer
      if (peer->hold_timer_interval > 0) {
        if (peer->ka != NULL) {
          peer->ka->reset();
        } else {
          peer->ka = new BGP_Timer(this, KeepAliveTimerExp, peer);
        }
        peer->ka->resched(peer->keep_alive_interval);
      }
      break;
    case BGP_Timer::MASO:
      printf("Min AS Origination Timer is unused!");
      break;
    case BGP_Timer::MRAI:
      printf("Invalid Min Route Advertisement Timer reset");
      break;
    default:
      printf("Unknown timer type: %d" + timertype);
    }
}

// ----- rtProtoBGP::ftadd ------------------------------------------------ //
  /**
   * Adds a route to the local forwarding table.
   *
   * @param info  Route information about the route to be added.
   */
void rtProtoBGP::ftadd(RouteInfo * info) {
    // Instead of adding route to the classifier through rtmodule,
    // we add route directly to the classifier here.
    if (!info->route->nlri->equals(bgp_id)) { // never add the local AS prefix
       int slot = fwd_table->classify(info->route->nexthop()->val());
       if ( slot != -1) {
         fwd_table->add_route(info->route->nlri->val(),
              info->route->nlri->prefix_len(),fwd_table->slot(slot));
       } else {
         cout << "Forwarding table adding error, unreachable next-hop: "
              << info->route->nexthop()->toString() << endl;
       }
    }    
}

  // ----- rtProtoBGP::ftrmv ------------------------------------------------ //
  /**
   * Removes a route to the local forwarding table.
   *
   * @param info  Route information about the route to be removed.
   */
void rtProtoBGP::ftrmv(RouteInfo * info) {
     fwd_table->del_route(info->route->nlri->val(),
          info->route->nlri->prefix_len());
}


 // ----- rtProtoBGP::handle_update ---------------------------------------- //
  /**
   * This method takes all necessary action when an update message is received.
   * This includes handling optional attributes, adding/removing entries from
   * Adj-RIBs-In, running the Decision Process, etc.
   *
   * @param msg An update message received by this BGP speaker.
   */
void rtProtoBGP::handle_update(UpdateMessage * msg) {
    // Extract each route from the update message (there will be one
    // route for each separate IP address prefix in the NLRI).
    PeerEntry * peer = nbs[msg->nh];
    peer->inupdates++;
    vector<Route *> rcvd_rtes = msg->rtes;
    vector<IPaddress *> rcvd_wds = msg->wds;

    // For now, no optional attributes are used,
    // so they don't need to be checked.
    bool rundp = false;  // whether or not to run the Decision Process
    vector<RouteInfo *> changedinfo;  // changed rtes to run DP Ph. 2 on

    // - - - - - - - check feasibility of new routes - - - - - - - //
    // check for cluster loops
    if (rcvd_rtes.size() > 0) {
      Route * rte = rcvd_rtes[0];
      if (reflector && peer->typ == PeerEntry::INTERNAL) {
        // this is a route reflector and has received an internal update
        if (peer->subtyp == PeerEntry::NONCLIENT) {
          // it was from a non-client, so check cluster list for loops
          if (rte->has_cluster_list() &&
              rte->cluster_list()->contains(cluster_num)) {
            // there was a loop, so all new routes in the update are infeasible
            for (int i=0; i<rcvd_rtes.size(); i++) {
              Route *r = rcvd_rtes[i];
              // treat infeasible route as a withdrawal
              rcvd_wds.push_back(r->nlri);
            }
            rcvd_rtes.clear();
          }
        }
      }
    }

    // check for AS path loops
    if (rcvd_rtes.size() > 0) {
      // All routes from the same update have the same ASpath, so just look at
      // first one.
      Route * rte = rcvd_rtes[0];
      if (rte->aspath()->contains(as_nh)) {
        // a loop exists, so all routes in this update are infeasible
        for (int i=0; i<rcvd_rtes.size(); i++) {
          Route * r = rcvd_rtes[i];
          // treat infeasible route as a withdrawal
          rcvd_wds.push_back(r->nlri);
        }
        rcvd_rtes.clear();
      }
    }

    // - - - - - - - - - - handle withdrawals - - - - - - - - - - //
    int num_wds = rcvd_wds.size();
    for (int i=0; i<num_wds; i++) {
      IPaddress * wd = rcvd_wds[i];

      // Remove the route from the appropriate Adj-RIB-In.
      // (If it's not actually in there, then no harm is done.)
      RouteInfo * rmvdinfo = peer->rib_in->remove(wd);
      if (rmvdinfo != NULL) {
        rmvdinfo->feasible = false; // mark it infeasible
        rundp = true;
        changedinfo.push_back(rmvdinfo);
      }
        
      // If there are any routes in the advertisement waiting lists of any peer
      // which 1) have the same destination as indicated by this withdrawal,
      // and 2) were received from the same peer as this withdrawal was
      // received from, then remove the info from the waiting list, as it is no
      // longer valid for advertisement.  (Note: For implicit withdrawals, we
      // need not worry about this situation because the new route which
      // replaces the old one will immediately be added to the wait list,
      // replacing the old one (if it was in the list).  This could be a
      // potential problem if filtering did not prevent the old route from
      // being advertised to a neighbor but does prevent the new route, and
      // thus the old one would not be replaced in the waiting list.  However,
      // a withdrawal will be issued which ought to take care of it.)
      for (int i=0;i<nbs_.size();i++) {
        PeerEntry * pe = nbs_[i];
        AdvPair adv_pair = pe->waiting_adv[wd->masked_val()]; // look for same destination
        if (adv_pair.rte != NULL && peer->nh == adv_pair.sender_nh) { 
          // Route has same destination and was from same peer, so remove.
          pe->waiting_adv.erase(wd->masked_val());
        }
      }
    }

    vector<RouteInfo*> newinfo_list;
    // - - - - - - - - - - handle new routes - - - - - - - - - - //
    for (int i=0; i<rcvd_rtes.size(); i++) {
      Route * rte = rcvd_rtes[i];
      // rundp = false;
      if (reflector && peer->typ == PeerEntry::INTERNAL) {
        // this is a route reflector and has received an internal update
        if (!rte->has_orig_id()) {
          // there is no ORIGINATOR_ID attribute, so add it
          rte->set_orig_id(peer->bgp_id);
        }
      }

      // If the update message contains a feasible route, it shall be placed in
      // the appropriate Adj-RIB-In, unless it is identical to a route which is
      // already in the Adj-RIB-In, in which case it is ignored.
      RouteInfo * newinfo = new RouteInfo(this,rte,RouteInfo::MIN_DOP,true,peer);
      RouteInfo * oldinfo = peer->rib_in->replace(newinfo);
      if (oldinfo != NULL && newinfo->route->equals(oldinfo->route)) {
        newinfo->inlocrib = true;
        continue; // they are identical, so skip it
      } else {
        newinfo_list.push_back(newinfo);
      }

      // i) If the NLRI is identical to the one of a route currently stored
      // in the Adj-RIB-In, then the new route shall replace the older route
      // in the Adj-RIB-In, thus implicitly withdrawing the older route from
      // service.  The BGP speaker shall run its Decision Process since the
      // older route is no longer available for use.
      if (oldinfo != NULL) { // we replaced an older route
        oldinfo->feasible = false; // the replacement is an implicit withdrawal
        changedinfo.push_back(oldinfo);
        rundp = true;
      }

      // ii) If the new route is an overlapping route that is included in an
      // earlier route contained in the Adj-RIB-In, the BGP speaker shall run
      // its Decision Process since the more specific route has implicitly
      // made a portion of the less specific route unavailable for use.
      ///ArrayList less_specifics = peer.rib_in.get_less_specifics(rte.nlri);
      vector<RouteInfo*> less_specifics = peer->rib_in->
                                                get_less_specifics(rte->nlri);
      if (less_specifics.size() > 0) { // there was a less specific route
        rundp = true;
        // Question: What if the path attributes are identical to that of one
        // of the less specific routes?  It seems like we wouldn't need to
        // run the DP (see iii below).
      }

      // iii) If the new route has identical path attributes to an earlier
      // route contained in the Adj-RIB-In, and is more specific than the
      // earlier route, no further actions are necessary.
      bool same_attribs = false;
      for (int j=0; j<less_specifics.size(); j++) {
        RouteInfo * ri = less_specifics[j];
        if (ri->route->equal_attribs(rte->pas)) {
          same_attribs = true;
        }
      }

      if (!same_attribs) {
        // iv) If the the new route has NLRI that is not present in [does not
        // overlap with] any of the routes currently stored in the Adj-RIB-In,
        // then the new route shall be placed in the Adj-RIB-In.  The BGP
        // speaker shall run its Decision Process.
        if (!peer->rib_in->is_less_specific(rte->nlri)) {
          rundp = true;
        } else {
          // v) If the new route is an overlapping route that is less
          // specific than an earlier route contained in the Adj-RIB-In, the
          // BGP speker shall run its Decision Process on the set of
          // destinations described only by the less specific route.
          rundp = true;
        }
      }
    } // end for each received route

    if (rundp) {
      for (int i=0; i<newinfo_list.size(); i++) {
        changedinfo.push_back(newinfo_list[i]);
      }
      decision_process_1(newinfo_list);
      vector<RouteInfo*> locribchanges = decision_process_2(changedinfo);
      decision_process_3(locribchanges);
    }
} // end of handle_update method

// ----- rtProtoBGP::dop -------------------------------------------------- //
  /**
   * Calculates the degree of preference of a route.  It is a non-negative
   * integer, and higher values indicate more preferable routes.
   *
   * @param rte  A route for which to calculate the degree of preference.
   * @return the degree of preference of the route
   */
int rtProtoBGP::dop(Route * rte) {
    int dop=0, numhops=0, i=0;

    // Currently, the degree of preference calculation works as follows.  If
    // the LOCAL_PREF attribute exists, then the value of LOCAL_PREF is used as
    // the DoP.  If not, the DoP is set to (100-n), where n is the number of AS
    // hops from this AS to the destination AS.  n can be calculated by
    // counting the number of ASs in the AS_PATH attribute.  A higher value for
    // DoP indicates a more preferable route.
    if (rte->has_localpref()) {
      dop = rte->localpref();
    } else {
      if (rte->aspath() != NULL) {
        for (int j=0; j<rte->aspath()->segs.size(); j++) {
          numhops += rte->aspath()->segs[j]->size();
        }
        dop = 100 - numhops;
      } else {
        // No AS_PATH, so must've been from internal peer advertising local AS
        // (and thus our AS prefix should be the same as the route's NLRI).
        if (!rte->nlri->equals(as_prefix)) {
          dop = RouteInfo::MIN_DOP; // assures that it is not selected
        } 
      }
    }
    return dop;
}

  // ----- rtProtoBGP::decision_process_1 ----------------------------------- //
  /**
   * Runs Phase 1 of the Decision Process, which is responsible for calculating
   * the degree of preference of newly added or updated routes.
   *
   * @param newinfo_list  A list of route information for which to calculate the
   *                  degrees of preference.
   */      
void rtProtoBGP::decision_process_1(vector<RouteInfo*> newinfo_list){
    for (int i=0; i<newinfo_list.size(); i++) {
      RouteInfo * info = newinfo_list[i];
      // Policy filtering is not supported by the current implementation.
      info->set_permissibility(true);
      // Calculate degree of preference whether or not the route was
      // permissible.  We probably don't actually have to bother for routes
      // which are not permissible, but just to be safe, I guess.
      info->dop = dop(info->route);
   }
}

  // ----- rtProtoBGP::remove_all_routes ------------------------------------ //
  /**
   * Removes from the Loc-RIB all routes learned from a given peer, then runs
   * Phases 2 and 3 of the Decision Process to replace the routes with backups
   * (if possible), and update neighbors with the changes.
   *
   * @param peer  The peer whose routes are to be invalidated.
   */ 
void rtProtoBGP::remove_all_routes(PeerEntry * peer) {
    vector<RouteInfo*> changedroutes = peer->rib_in->remove_all();
    for (int i=0; i<changedroutes.size(); i++) {
      RouteInfo * ri = changedroutes[i];
      ri->feasible = false;
    }
    // Run Decision Process Phase 2, since removing all routes from a
    // particular peer (which usually results from peering session termination)
    // is essentially identical to receiving withdrawals for every route
    // previously advertised by that peer.
    vector<RouteInfo*> locribchanges = decision_process_2(changedroutes);
    decision_process_3(locribchanges);
}


  // ----- rtProtoBGP::decision_process_2 ----------------------------------- //
  /**
   * Runs Phase 2 of the Decision Process, which is responsible for selecting
   * which routes (from Adj-RIBs-In) should be installed in Loc-RIB.
   *
   * @param changedroutes  A list of info on recent route changes.
   * @return a vector of route changes made to the Loc-RIB
   */
vector<RouteInfo*> rtProtoBGP::decision_process_2(vector<RouteInfo*> changedroutes){   
    vector<RouteInfo*> locribchanges; // changes to Loc-RIB (for DP3)

    for (int i=0; i<changedroutes.size(); i++) {
      RouteInfo *info = changedroutes[i];
      // - - - - - withdrawals - - - - -
      if (!info->feasible) { // an infeasible route
        if (info->inlocrib) { // it was in the Loc-RIB
          RouteInfo * oldinfo = loc_rib->remove(info->route->nlri);
          locribchanges.push_back(info);

          // We removed a route from the Loc-RIB.  See if we can replace it
          // with another route (with the same NLRI) from the Adj-RIBs-In.
          // (And if there's more than one choice, find the most preferable.)
          RouteInfo * bestnewinfo = NULL;
          for (int i=0;i<nbs_.size();i++) {
            PeerEntry * peer = nbs_[i];
            RouteInfo * tmpinfo = peer->rib_in->find(info->route->nlri);
            if (tmpinfo != NULL && tmpinfo->permissible) {
              if (bestnewinfo == NULL || tmpinfo->compare(bestnewinfo) > 0) {
                bestnewinfo = tmpinfo;
              }
            }
          }
          if (bestnewinfo != NULL) {
            // We found a replacement for the withdrawn route.  Keep in mind
            // that we have not yet checked any newly advertised routes, which
            // may be better than the replacement we just found.  Those will be
            // checked in the 'advertisements' section of code below.
            loc_rib->add(bestnewinfo);
            locribchanges.push_back(bestnewinfo);
          }
        }
      } else { // it's a feasible route
        // - - - - - advertisements - - - - -
        if (info->permissible) { // our policy allows it
          // See if this new feasible, permissible route is better than the
          // current route with the same NLRI in Loc-RIB (if one exists).
          RouteInfo * curinfo = loc_rib->find(info->route->nlri);        
          if (curinfo == NULL || info->compare(curinfo) > 0) {
            bool found_ad = false, found_wd = false;
            if (curinfo != NULL) { // we're about to replace Loc-RIB info
              loc_rib->remove(info->route->nlri);

              // It's possible that we handled a withdrawal for this very route
              // just a moment ago (in the 'withdrawals' section of code
              // above).  If that is the case, then we may also have, at that
              // time, found a replacement for the route already.  If so, then
              // at this point, the newly advertised route (which was likely,
              // but not necessarily, an implicit withdrawal) is about to
              // replace that replacement which was found above.  Rather than
              // considering this as two changes to the Loc-RIB, it would
              // simplifiy things to treat it as just one, since they're
              // happening simultaneously.  So here we check to see if we are
              // in fact about to replace a replacement.
              for (int j=0; j<locribchanges.size(); j++) {
                RouteInfo * ri = locribchanges[j];
                if (ri->route->nlri->equals(info->route->nlri)) {
                  if (ri->feasible) {
                    // We can leave the Loc-RIB change regarding the
                    // withdrawal, but should overwrite the Loc-RIB change
                    // regarding the new route.
                    loc_rib->add(info);
                    locribchanges[j] = info;
                    found_ad = true;
                  } else {
                    found_wd = true;
                  }
                }
              }

              if (!found_ad) { // See big comment block above.
                locribchanges.push_back(curinfo);
              }             
            }
            if (!found_ad) { // See big comment block above.
              loc_rib->add(info);
              locribchanges.push_back(info);
            }
          } 
        } 
      }
    }
    return locribchanges;
}

  // ----- BGPSession.decision_process_3 ----------------------------------- //
  /**
   * Runs Phase 3 of the Decision Process, which is responsible for
   * disseminating routes to peers.  This is done by inserting certain routes
   * from Loc-RIB into Adj-RIBs-Out.
   *
   * @param locribchanges  A list of changes to the Loc-RIB.
   */
void rtProtoBGP::decision_process_3(vector<RouteInfo*> locribchanges){
    // Normally executed after Phase 2, but must also be executed when routes
    // to local destinations (in Loc-RIB) have changed, when locally generated
    // routes learned by means outside of BGP have changed, or when a new
    // peering session has been established.
    
    map<PeerEntry*,vector<IPaddress*> > wds_tbl;
    map<PeerEntry*,vector<RouteInfo*> > ads_tbl;

    // handle withdrawals first
    for (int i=0;i<nbs_.size();i++) {
      PeerEntry * peer = nbs_[i];
      vector<IPaddress*> wds2send;
      for (int i=0; i<locribchanges.size(); i++) {
        RouteInfo * info = locribchanges[i];
        if (!info->inlocrib && advertisable(info, peer)) {
          // We're not using the route any more, so withdraw it.
          RouteInfo * oldinfo = peer->rib_out->remove(info->route->nlri);          
          if (peer->connected) {
             wds2send.push_back(new IPaddress(info->route->nlri));
          } // else no updates have yet been sent to this peer, so no need
            // to send withdrawals
        }
      }
      wds_tbl[peer] = wds2send ;
    }

    // Handle new routes next. (Must do after all withdrawals since new routes
    // imply withdrawal and thus we remove any withdrawals with the same NLRI.)
    for (int i=0;i<nbs_.size();i++) {
      PeerEntry * peer = nbs_[i];
      vector<RouteInfo*> ads2send;
      vector<IPaddress*> wds2send = wds_tbl[peer];  
      for (int i=0; i<locribchanges.size(); i++) {

        RouteInfo * info = locribchanges[i];
        if (info->inlocrib && advertisable(info, peer)) {
          // it's a route that we started using
          peer->rib_out->replace(info); // will replace previous, if any
          ads2send.push_back(info);
          // we have to operate on wds_tbl[peer] directly
          for (int k=0; k<wds_tbl[peer].size(); k++) { // adv implies withdrawal
            if (info->route->nlri->equals(wds_tbl[peer][k])) {
                wds_tbl[peer].erase(wds_tbl[peer].begin()+k);         
            }
          }
        }
      }
      ads_tbl[peer] = ads2send;
    }
    
    // update the connectedness of each neighbor
    for (int i=0;i<nbs_.size();i++) {
      PeerEntry * peer = nbs_[i];
      if (!peer->connected) {
        // As of the last update of the Adj-RIBs-Out, there was no peering
        // session with this neighbor (or there was no previous update--this is
        // the first).

        if (peer->connection_state == rtProtoBGP::ESTABLISHED) {
          // There is a new peering session with this neighbor.  Rather than
          // just sending the newest changes to the Loc-RIB, we want to send
          // everything that's in the Adj-RIB-Out.
          peer->connected = true;
          vector<RouteInfo*> ads2send = peer->rib_out->get_all_routes();
          ads_tbl[peer] = ads2send;
        }
      }
    }

    external_update(wds_tbl, ads_tbl);
}

  // ----- rtProtoBGP::external_update -------------------------------------- //
  /**
   * Tries to send update messages to each external peer if there is any new
   * route information in Adj-RIBs-Out to be shared with them.  Currently, this
   * method also handles updating internal peers.
   *
   * @param wds_table  A table of NLRI of withdrawn routes which need to be sent.
   * @param ads_table  A table of routes which need to be advertised.
   */
void rtProtoBGP::external_update(map<PeerEntry*,vector<IPaddress*> > wds_table,
                     map<PeerEntry*,vector<RouteInfo*> > ads_table) {
    map<PeerEntry*,vector<AdvPair> > new_ads_table;
    // First, make copies of the routes and modify them if necessary.
    for (int i=0;i<nbs_.size();i++) {
      PeerEntry * rcvr = nbs_[i];
      vector<RouteInfo*> ads = ads_table[rcvr]; // a list of RouteInfos
      vector<AdvPair> newads;  // this will be a list of Pairs
      for (int i=0; i<ads.size(); i++) {
        RouteInfo * info = ads[i];
        PeerEntry * sender = info->peer;
        Route * newrte = new Route(info->route); // make copy to put in update
        // ----- make any necessary modifications to the route -----
        if (reflector && sender->typ == PeerEntry::INTERNAL &&
            rcvr->typ == PeerEntry::INTERNAL &&
            (rcvr->bgp_id == NULL || // no peering session yet, so not originator
             !info->route->has_orig_id() || // no originator ID attribute exists
             !rcvr->bgp_id->equals(info->route->orig_id())) && // not originator
            (sender->subtyp == PeerEntry::CLIENT ||
             rcvr->subtyp == PeerEntry::CLIENT)) { // reflecting
          if (rcvr->subtyp == PeerEntry::NONCLIENT) { // to a non-client
            newrte->append_cluster(cluster_num); // append cluster number
          }
        } else { // not reflecting
          newrte->set_nexthop(rcvr->return_ip); // set next hop
        }
        if (rcvr->typ == PeerEntry::EXTERNAL) { // sending to external peer
          newrte->prepend_as(as_nh); // add my AS to the AS path
          newrte->remove_attrib(LocalPref::TYPECODE);
        }
        AdvPair adv_pair;
        adv_pair.rte = newrte;
        adv_pair.sender_nh = sender->nh;
        newads.push_back(adv_pair);
      }
      new_ads_table[rcvr] = newads; // replace entry with new routes
    }

    // put together update messages to send
    for (int i=0;i<nbs_.size();i++) {
      PeerEntry * peer = nbs_[i];
      vector<IPaddress*> wds = wds_table[peer];
      vector<AdvPair> ads = new_ads_table[peer]; // a list of Pairs
      if (peer->connected) {
        UpdateMessage * msg = new UpdateMessage(nh);
        for (int i=0; i<wds.size(); i++) {
          msg->add_wd(wds[i]);
        }
        if (ads.size() > 0) {
          msg->add_route(ads[0].rte);
          vector<string> senders;
          senders.push_back(ads[0].sender_nh);
          try_send_update(msg,senders,peer);
          for (int i=1; i<ads.size(); i++) {
            msg = new UpdateMessage(nh, ads[i].rte);
            senders.clear();
            senders.push_back(ads[i].sender_nh);
            try_send_update(msg,senders,peer);
          }
        } else if ( msg->wds.size() > 0) { // withdrawals only
          vector<string> senders;
          senders.clear();
          try_send_update(msg,senders,peer);
        } // else neither announcements nor withdrawals
      } 
    }    
}

  // ----- rtProtoBGP::advertisable ----------------------------------------- //
  /**
   * Determines if a route should be advertised to a particular peer.
   *
   * @param info  The route in question.
   * @param rcvr  The peer to whom the route may be advertised.
   * @return true only if the route should be advertised to the given peer
   */
bool rtProtoBGP::advertisable(RouteInfo * info, PeerEntry * rcvr) {
    PeerEntry * sender = info->peer; // who sent it to us
    if (sender == rcvr) {
      return false; // don't advertise back to sender
    }
    
    // - - - - - sender-side loop detection - - - - - //
    if (Global::ssld) {
      if (info->route->aspath()->contains(rcvr->as_nh)) {
        // A loop would exist for our peer, so don't send it.
        return false;
      }
    }

    if (sender->typ == PeerEntry::EXTERNAL) {
      return true; // route was received externally
    }

    if (sender == self && rcvr->typ == PeerEntry::INTERNAL) {
      // it's the route to our own AS, which internal peers will already know
      return false;
    }

    if (rcvr->typ == PeerEntry::EXTERNAL) {
      // we received the route internally, but the peer to send to is external
      return true;
    }

    if (reflector) {
      if (sender->subtyp == PeerEntry::CLIENT) {
        if (!info->route->has_orig_id() || // no originator ID attribute
            !rcvr->bgp_id->equals(info->route->orig_id())) { // not originator
          // The route was received internally, but this is a route reflector,
          // the route was sent to us by a client, and the peer to send to was
          // not the originator.
          return true;
        } else {
          // the peer to send to was the originator, don't forward
          return false;
        }
      } else if (rcvr->subtyp == PeerEntry::CLIENT) {
        if (rcvr->bgp_id == NULL || // no peering session yet, so not originator
            !info->route->has_orig_id() || // no originator ID attribute exists
            !rcvr->bgp_id->equals(info->route->orig_id())) { // not originator
          // The route was received internally, but this is a route reflector
          // and though it was sent to us by a reflector non-client, the peer
          // to send to is a reflector client (and was not the originator) so
          // it's OK.
          return true;
        } else {
          // the peer to send to was the originator, don't forward
          return false;
        }
      } else {
        // Route not being forwarded.  Both the peer that sent it to us and the
        // peer to send to are internal (reflector) non-clients.
        return false;
      }
    } else {
      // Route not being forwarded.  It was received internally,
      // the peer is internal, and this is not a route reflector.
      return false;
    }
}

  // ----- rtProtoBGP::sendmsg ---------------------------------------------- //
  /**
   * Does the actual pushing of a message to the protocol below this one on the
   * protocol stack.  The send method is just a public interface
   * for sending messages, and if outgoing messages are being delayed with
   * jitter, it will not actually send the message but simply add it to the
   * queue of outgoing messages.
   *
   * @param msg   The BGP message to be sent out.
   * @param peer  The entry for the peer to whom the message should be sent.
   */
void rtProtoBGP::send(BGPMessage * msg, PeerEntry * peer) {
    peer->send(msg);
}                         

  // ----- rtProtoBGP::try_send_update -------------------------------------- //
  /**
   * Handles the sending of an update message.  If for any reason it cannot be
   * sent right away, it takes the proper actions.
   *
   * @param msg      The update message to send.
   * @param senders  The NHI addresses of the senders of each route in the
   *                 update message; this information is required if the route
   *                 cannot be advertised right away.
   * @param peer     The peer to whom the message should be sent.
   */
void rtProtoBGP::try_send_update(UpdateMessage * msg, vector<string> senders,
                                    PeerEntry * peer) {
                                      
    if (!rate_limit_by_dest && peer->mraiTimer->is_pending()) {
      // We're doing rate limiting by peer only, and the MRAI timer is not yet
      // expired so no advertisments can be sent.  Any prefixes to be
      // advertised must be put in the waiting list.  However, if rate limiting
      // is not being applied to withdrawals, any withdrawn routes may be sent.
      // Otherwise, the prefixes to be withdrawn must be put on the withdrawals
      // waiting list.  Before doing any of this, though we must check a couple
      // of things:

      // - - - - - remove redundant withdrawals - - - - - //
      if (msg->rtes.size() > 0) {   // the message contains NLRI
        // First we need to check the following.  If there's a prefix D in both
        // the NLRI and the withdrawn routes then the new advertisement will
        // suffice to serve as both the withdrawal and the new advertisement.
        // (whether or not withdrawal rate limiting is being used).  In that
        // case, we remove the withdrawn route from the message.     
        IPaddress * nlri;
        if (msg->wds.size() > 0) {
          for (int i=0; i<msg->rtes.size(); i++) {
            nlri = (msg->rtes[i])->nlri;
            if (msg->remove_wd(nlri) && Global::DEBUG) {
              cout << " Just removed redundant withdrawals for nlri: "
                   << nlri->toString() << endl;
            }
          }
        }
      }
      
      // - - - - - updating waiting routes list - - - - - //
      if (msg->wds.size() > 0) { // the msg contains withdrawals
        // Next, make sure there were no routes with the withdrawn destinations
        // in the waiting routes list.  (If so, remove them.)
         for (int i=0; i<msg->wds.size(); i++) {
          AdvPair wrtepair = peer->waiting_adv[msg->wds[i]->masked_val()];
          peer->waiting_adv.erase(msg->wds[i]->masked_val());          
        }
      }      

      // Finally, we can go ahead with putting the new prefixes in the waiting
      // lists and removing them from the update message.
      if (msg->rtes.size() > 0) {
        for (int i=msg->rtes.size()-1; i>=0; i--) {
          IPaddress * nlri = (msg->rtes[i])->nlri;
          AdvPair newPair;
          newPair.rte = msg->rtes[i];
          newPair.sender_nh = senders[i];  
          peer->waiting_adv[nlri->masked_val()] = newPair;
          msg->rtes.erase(msg->rtes.begin()+i);
        }
      }
      
      if (Global::wrate) {
        // We're applying rate limiting to withdrawals, so go ahead and
        // stick prefixes to be withdrawn in the waiting list.
        if (msg->wds.size() > 0) {
          for (int i=0; i<msg->wds.size(); i++) {
            IPaddress * wdrte = msg->wds[i];
            peer->waiting_wds[wdrte->masked_val()] = wdrte;
          }
        }
      } else {
        // We can send the update with withdrawn routes only.
        // We may have just removed some withdrawals and/or routes from the
        // message--if it's now completely empty then don't sent it!
        if (msg->wds.size() > 0 ) { // the message is non-empty
          send(msg, peer);
          reset_timer(peer, BGP_Timer::KEEPALIVE); // reset the KeepAlive timer
        }
      }
      return;
    }

    // -- -- -- -- not applying MRAI to withdrawals -- -- -- -- //
    if (!Global::wrate) {                                               
      // - - - - - remove redundant withdrawals - - - - - //
      if (msg->rtes.size() > 0) { // the message contains NLRI
        // First we need to check the following.  If we are advertising a route
        // to destination D and also withdrawing an old route to destination D,
        // then the new advertisement will suffice to serve as both the
        // withdrawal and the new advertisement (whether or not the update is
        // put on the wait list).  In that case, we remove the withdrawn route
        // from the message.
        IPaddress * nlri;
        if (msg->wds.size() > 0) {
          for (int i=0; i<msg->rtes.size(); i++) {
            nlri = (msg->rtes[i])->nlri;
            if (msg->remove_wd(nlri) && Global::DEBUG) {
              cout << " Just removed redundant withdrawals for nlri: "
                   << nlri->toString() << endl;
            }
          }
        }
      }

      // - - - - - updating waiting routes list - - - - - //
      if (msg->wds.size() > 0) { // the msg contains withdrawals
        // Make sure there were no routes with the withdrawn destinations in
        // the waiting routes list.  (If so, remove them.)
        for (int i=0; i<msg->wds.size(); i++) {         
          if (msg->wds[i] != NULL) {
            AdvPair wrtepair = peer->waiting_adv[msg->wds[i]->masked_val()];
            peer->waiting_adv.erase(msg->wds[i]->masked_val());       
          } 
        }
      }
      
      // - - - - - check Minimum Route Advertisement Interval - - - - - //
      if (msg->rtes.size() > 0 && rate_limit_by_dest) { // the msg contains NLRI
        if (peer->mrai > 0) {
          IPaddress * nlri;
          for (int i=msg->rtes.size()-1; i>=0; i--) {
            nlri = (msg->rtes[i])->nlri;
            if ((peer->adv_nlri)[nlri->masked_val()] != NULL) {
              // Can't send this route right now (since another with the same
              // NLRI was sent to the same peer recently), so remove it from
              // the update message and put it on the waiting list.  Note that
              // if there was already a route with the same NLRI on the waiting
              // list, it will be replaced.
              AdvPair newPair;
              newPair.rte = msg->rtes[i];
              newPair.sender_nh = senders[i];  
              peer->waiting_adv[nlri->masked_val()] = newPair;
              msg->rtes.erase(msg->rtes.begin()+i);
            }
          }
        }
      }               

      // - - - - - send the message - - - - - //
      // We may have just removed some withdrawals and/or routes from the
      // message--if it's now completely empty then don't sent it!
      if (msg->wds.size() > 0 || msg->rtes.size() > 0) { // the message is non-empty
        send(msg, peer);
        reset_timer(peer, BGP_Timer::KEEPALIVE); // reset the KeepAlive timer

        if (msg->rtes.size() > 0 && rate_limit_by_dest && peer->mrai > 0) {
          // add routes to sent routes table
          MRAITimer * tmr;
          for (int i=0; i<msg->rtes.size(); i++) {
            Route * rte = msg->rtes[i];
            peer->adv_nlri[rte->nlri->masked_val()] = rte->nlri;
            tmr = new MRAITimer(this, rte->nlri, peer);
            tmr->resched((double) peer->mrai);
            peer->mrais[rte->nlri->masked_val()] = tmr;
          }
        } else if (msg->rtes.size() > 0 && !rate_limit_by_dest && peer->mrai > 0) {
          peer->mraiTimer->reset();
          peer->mraiTimer->resched((double) peer->mrai);
        }
      }

    } else {
    // -- -- -- -- applying MRAI to withdrawals -- -- -- -- //
      // - - - - - remove redundant withdrawals - - - - - //
      if (msg->rtes.size() > 0) {
        // First we need to check the following.  If we are advertising a route
        // to destination D and also withdrawing an old route to destination D,
        // then the new advertisement will suffice to serve as both the
        // withdrawal and the new advertisement (whether or not the update is
        // put on the wait list).  In that case, we remove the withdrawn route
        // from the message.
        IPaddress * nlri;
        if (msg->wds.size() > 0) {
          for (int i=0; i<msg->rtes.size(); i++) {
            nlri = (msg->rtes[i])->nlri;
            msg->remove_wd(nlri);
          }
        }
      }

      // - - - - - check advertisements against MRAI - - - - - //
      if (msg->rtes.size() > 0 && rate_limit_by_dest) {
        if (peer->mrai > 0) {
          IPaddress * nlri;
          for (int i=msg->rtes.size()-1; i>=0; i--) {
            nlri = (msg->rtes[i])->nlri;
            if (peer->adv_nlri.find(nlri->val()) != peer->adv_nlri.end() ||
                peer->wdn_nlri.find(nlri->val()) != peer->wdn_nlri.end() ) {   
              // Can't send this route right now (since an advertisement or a
              // withdrawl with the same NLRI was sent to the same peer recently),
              // so remove it from the update message and put it on the waiting
              // list.  If a withdrawal with the same NLRI is in the withdrawal
              // waiting list, it must be removed.  Note that if there was already
              // a route with the same NLRI on the advertisement waiting list, it
              // will be replaced.
              AdvPair newPair;
              newPair.rte = msg->rtes[i];
              newPair.sender_nh = senders[i];  
              peer->waiting_adv[nlri->masked_val()] = newPair;
              msg->rtes.erase(msg->rtes.begin()+i); 
              peer->waiting_wds.erase(nlri->val());
            }   
          }
        }
      }

      // - - - - - check withdrawals against MRAI  - - - - - //
      if (msg->wds.size() > 0 && rate_limit_by_dest) {
        if (peer->mrai > 0) {
          IPaddress * wdnlri;
          for (int i=msg->wds.size()-1; i>=0; i--) {
            wdnlri = msg->wds[i];
            if (peer->adv_nlri.find(wdnlri->val()) != peer->adv_nlri.end() ||
                peer->wdn_nlri.find(wdnlri->val()) != peer->wdn_nlri.end() ) {
              // Can't send this withdrawal right now (since an advertisement
              // with the same NLRI was sent to the same peer recently), so
              // remove it from the update message and put it on the waiting
              // list.  If an advertisement with the same NLRI is in the
              // advertisement waiting list, it must be removed.  Note that if
              // there was already an entry with the same NLRI on the
              // withdrawal waiting list, it will be replaced.
              peer->waiting_wds[wdnlri->masked_val()] = wdnlri;
              msg->wds.erase(msg->wds.begin()+i); 
              peer->waiting_adv.erase(wdnlri->val());
            } 
          }
        }
      }

      // - - - - - send the message - - - - - //

      // We may have just removed some withdrawals and/or routes from the
      // message--if it's now completely empty then don't sent it!
      if (msg->wds.size() > 0 || msg->rtes.size() > 0) { // the message is non-empty
        send(msg, peer);
        reset_timer(peer, BGP_Timer::KEEPALIVE); // reset the KeepAlive timer
        if (msg->rtes.size() > 0 && rate_limit_by_dest && peer->mrai > 0) {
          // add routes to sent routes table
          MRAITimer * tmr;
          for (int i=0; i<msg->rtes.size(); i++) {
            Route * rte = msg->rtes[i];
            peer->adv_nlri[rte->nlri->masked_val()] = rte->nlri;
            tmr = new MRAITimer(this, rte->nlri, peer);
            tmr->resched((double) peer->mrai);
            peer->mrais[rte->nlri->masked_val()] = tmr;
          }
        }

        if (msg->wds.size() > 0 && rate_limit_by_dest && peer->mrai > 0) {
          // add withdrawn prefixes to sent withdrawn prefixes table
          MRAITimer * tmr;
          for (int i=0; i<msg->wds.size(); i++) {
            IPaddress * wdpref = msg->wds[i];
            peer->wdn_nlri[wdpref->masked_val()] = wdpref;
            tmr = new MRAITimer(this, wdpref, peer);
            tmr->resched((double) peer->mrai);
            peer->mrais[wdpref->masked_val()] = tmr;
          }
        }

        if (!rate_limit_by_dest && peer->mrai > 0) {
          peer->mraiTimer->resched((double) peer->mrai);
        }
      }
    }
} // end of try_send_update method

  // ----- rtProtoBGP::listen ----------------------------------------------- //
  /**
   * Wait for a completed socket connection (with a neighbor).
   */
void rtProtoBGP::listen() {
    if (!alive) {
      printf("socket listen attempted while dead\n");
      return;
    }

    class AcceptCon : public Continuation  {
    private:
       TcpSocket ** mynewsocket;
       rtProtoBGP * bgp;
    public:
        AcceptCon (TcpSocket ** newsocket,rtProtoBGP * b) : Continuation(NULL) {
          mynewsocket = newsocket;
          bgp = b;
        }
        ~AcceptCon() {}
        inline  virtual void success() {
            PeerEntry * nb = (bgp->nbsip)[(unsigned int)mynewsocket[0]
                                                                ->dest_ip_addr];
            if (!bgp->alive || nb->readsocket != NULL) {
                if (!bgp->alive) {
                  cout << "closing new readsocket connection with " +
                             nb->nh + ": BGP is dead!" << endl;
                } else {
                  cout << "closing new readsocket connection with " +
                             nb->nh + ": already have one" << endl;
                }
            }
            nb->readsocket = mynewsocket[0];  // now we can listen
            nb->receive(); // now we can listen
            if (nb->writeconnected[nb->writesocket]) {
               bgp->push(new TransportMessage(TransConnOpen, nb->nh));
            } else { // don't yet have outgoing connection
              if (!nb->writeconnecting[nb->writesocket]) {
                // haven't yet tried to get current valid outsocket
                nb->connect();
              }
            }
            // As long as the BGP process hasn't crashed, always keep listening
            // for more requests.  (We don't want to place a limit since
            // connections can go down and come back up again ad infinitum.)
            if (bgp->alive) { // if BGP hasn't gone down
              bgp->listen();
            }
        }
        inline  virtual void failure() {
          // connection request from potential peer failed before servicing
          if (bgp->alive) { // if BGP hasn't gone down
              printf("failure accepting socket connection. \n");
              bgp->listen();
          } // else the reason for failure was probably because BGP died
        }
     };

     TcpSocket ** newsocket = new TcpSocket* [1];
     AcceptCon * acceptCon = new AcceptCon(newsocket,this);
     listensocket->accept(newsocket, acceptCon);
}

// ----- rtProtoBGP::push ------------------------------------------------- //
  /**
   * This process passes BGP events on to the receive method to be handled.  All
   * thirteen types of events (both externally and internally generated) pass
   * through this method in the BGP flow of control.  For externally generated
   * events, push method is not called by the protocol directly below BGP
   * (which is Sockets) to pass a message up, but is called by BGP methods
   * which are reading from sockets.  
   *
   * @param message      The incoming event/message.
   * @return true if the method executed without error
   */
bool rtProtoBGP::push(BGPMessage * message) {

    if (!alive) { // if the BGP process is dead
      if (message->type == BGPMessage::RUN) {
        // The only "message" that is recognized in the dead state is a "run"
        // directive.
        alive = true;
        // Add statically configured routes to the Loc-RIB.  (For now all that
        // means is to add one route for our AS's IP prefix.)
        vector<RouteInfo*> locribchanges;
        for (int i=0; i<static_rtes.size(); i++) {
          Route * rte = new Route();
          rte->set_nlri(static_rtes[i]);
          rte->set_origin(Origin::IGP);
          rte->set_nexthop(bgp_id);
          RouteInfo * info = new RouteInfo(this,rte,RouteInfo::MAX_DOP,
                                             true,self);
          loc_rib->add(info);
          locribchanges.push_back(info);
          if (Global::auto_advertise) {
            // By inserting a route to this AS in the Loc-RIB and then starting
            // Phase 3 of the Decision Process, we effectively cause update
            // messages to be sent to each of our peers.  Note that we insert
            // into the Loc-RIB but *not* into the local router's forwarding
            // table.
            // run Phase 3 of the Decision Process so that the changes to the
            // Loc-RIB will get propagated to the Adj-RIBs-Out.
            decision_process_3(locribchanges);
          }
         }
        listensocket = new TcpSocket();
        listensocket->setTcpMaster(tcpmaster);
        listensocket->bind(bgp_id->intval(), PORT_NUM);
        listensocket->listen();
        listen();
        
        // Send a BGPstart event for each potential external peering session
        // and for each potential internal peering session.  This will cause
        // BGP to start actively trying to connect to neighbors.
        for (int i=0;i<nbs_.size();i++) {
          PeerEntry * peer = nbs_[i];
          push(new StartStopMessage(BGPstart,peer->nh));
        }
      }
      return true;
    }
    return receive(message);
}

  // ----- rtProtoBGP::receive ---------------------------------------------- //
  /**
   * This process receives and handles both externally and internally generated
   * BGP events.  
   *
   * @param message      The incoming event/message.
   * @return true if the method executes without error
   */
bool rtProtoBGP::receive(BGPMessage * message) {
    if (!alive) { return true; } // ignore all (external) messages while dead
    BGPMessage * msg = message;

    if (msg->type == BGPMessage::RUN) {
      printf("run directive received while running");
      return true;
    }

    int event_type;
    // Get the peer with whom this message is associated.
    PeerEntry * peer = nbs[msg->nh];
    if (peer == NULL) {
      cout << "unknown neighbor: " + msg->nh;
      return false;
    }
    
    // This switch statement is used mainly to set the event_type parameter,
    // though it also handles a few other message-type-specific issues.
    switch (msg->type) {
    case BGPMessage::OPEN:
      event_type = RecvOpen;
      // Since we don't start out with full information about each
      // peer, we need to add it as we hear from them.
      if (peer->typ == PeerEntry::INTERNAL) {
        if( peer->as_nh != ((OpenMessage*)msg)->as_nh) {
           printf("Unexpected AS mismatch. \n");
         }  
      } else {
        peer->as_nh  = ((OpenMessage*)msg)->as_nh;
      }
      peer->bgp_id   = ((OpenMessage*)msg)->bgpid;
      break;
    case BGPMessage::UPDATE:        event_type = RecvUpdate;          break;
    case BGPMessage::NOTIFICATION:  event_type = RecvNotification;    break;
    case BGPMessage::KEEPALIVE:     event_type = RecvKeepAlive;       break;
    case BGPMessage::TIMEOUT:
      event_type = ((TimeoutMessage*)msg)->to_type;                   break;
    case BGPMessage::TRANSPORT:
      event_type = ((TransportMessage*)msg)->trans_type;              break;
    case BGPMessage::STARTSTOP:
      event_type = ((StartStopMessage*)msg)->ss_type;                 break;
    default:
      printf("Illegal BGP message type. \n");
      event_type = -1; // to avoid compiler errors
    }

    if (Global::DEBUG) {
      cout << endl;
      cout << " time: " << now() << endl;
      cout << " router: " << peer->return_ip->val2str() <<
              " received msg from peer: " << peer->ip_addr->val2str() << endl;     
      cout << " event_type: *" << eventNames[event_type] << "*" << endl;
      cout << " connection_state: *" << statestr[peer->connection_state]
           << "*" << endl;
      cout << " received msg: " << msg->toString() << endl;
    }  

    vector<RouteInfo*> locribchange;
    BGP_Timer * st;
    // switch based on the state of the BGP connection with the sender
    switch (peer->connection_state) {
    // - - - - - - - - - - - - - - - IDLE - - - - - - - - - - - - - - - - //
    case IDLE:
      switch (event_type) {
      case BGPstart:
        // 1. start ConnectRetry timer
        // 2. initiate a transport connection

        reset_timer(peer, BGP_Timer::CONNRETRY);
        peer->connect();
        peer->connection_state = CONNECT;
        break;
      case KeepAliveTimerExp:
        printf("rcvd KeepAlive Timer Expired msg while Idle");
        break;
      case HoldTimerExp:
        printf("rcvd Hold Timer Expired msg while Idle");
        break;
      case TransConnOpen:
        printf("rcvd Transport Connection Open msg while Idle");
        break;
      default:
        cout << "ignoring msg from bgp@" + peer->nh +
                  " rcvd while Idle. " ; 
      }
      break;
      
    // - - - - - - - - - - - - - - - CONNECT - - - - - - - - - - - - - - - //
    case CONNECT:
      switch (event_type) {
      case BGPstart: // ignore
        break;
      case TransConnOpen:
        // 1. clear ConnectRetry timer
        // 2. send OPEN message                 

        peer->crt->cancel();
        send(new OpenMessage(bgp_id, as_nh, nh,
                             peer->hold_timer_interval), peer);
        // RFC1771 section 8 suggests setting the Hold Timer to 4 minutes here
        peer->ht = new BGP_Timer(this, HoldTimerExp, peer);
        peer->ht->resched(240.0);
        peer->connection_state = OPENSENT;
        break;
      case TransConnOpenFail:
        // 1. restart ConnectRetry timer

        peer->close(); // close the sockets that couldn't connect
        reset_timer(peer, BGP_Timer::CONNRETRY);
        peer->connection_state = ACTIVE;
        break;
      case ConnRetryTimerExp:
        // 1. restart ConnectRetry timer
        // 2. initiate a transport connection

        reset_timer(peer, BGP_Timer::CONNRETRY);
        if (!peer->writeconnecting[peer->writesocket]) {
          peer->connect();
        } // else the previous connect() is still trying        
        break;
      default: // for BGPstop, TransConnClosed, TransFatalError,
               // HoldTimerExp, KeepAliveTimerExp, RecvOpen,
               // RecvKeepAlive, RecvUpdate, RecvNotification
        // 1. release resources

        peer->close();
        peer->cancel_timers();
        peer->connection_state = IDLE;
        if (Global::auto_reconnect) {             // another Globle configure flag
          push(new StartStopMessage(BGPstart,peer->nh));
        }
      }
      break;      
      
    // - - - - - - - - - - - - - - - ACTIVE - - - - - - - - - - - - - - - //
    case ACTIVE:
      switch (event_type) {
      case BGPstart: // ignored
        break;
      case TransConnOpen:
        // 1. clear ConnectRetry timer
        // 2. send OPEN message

        peer->crt->cancel();
        send(new OpenMessage(bgp_id, as_nh, nh,
                             peer->hold_timer_interval), peer);
        // RFC1771 section 8 suggests setting the Hold Timer to 4 minutes here
        peer->ht = new BGP_Timer(this, HoldTimerExp, peer);
        peer->ht->resched(240.0);
        peer->connection_state = OPENSENT;
        break;
      case TransConnOpenFail:
        // 1. close connection
        // 2. restart ConnectRetry timer

        peer->close(); // close the sockets that couldn't connect
        reset_timer(peer, BGP_Timer::CONNRETRY);
        break;
      case ConnRetryTimerExp:
        // 1. restart ConnectRetry timer
        // 2. initiate a transport connection

        reset_timer(peer, BGP_Timer::CONNRETRY);
        peer->connect();
        // It is safe to call connect() again here because the previous call
        // must necessarily have completed.  The only two ways to get to into
        // the Active state (with TransConnOpenFail or TransConnClose) require
        // that the call completed.
        peer->connection_state = CONNECT;
        break;
      default: // for BGPstop, TransConnClosed, TransFatalError,
               // HoldTimerExp, KeepAliveTimerExp, RecvOpen,
               // RecvKeepAlive, RecvUpdate, RecvNotification     
        peer->close();
        peer->cancel_timers();
        peer->connection_state = IDLE;
        if (Global::DEBUG)
          cout << " BGP session with peer " <<  peer->ip_addr->val2str()
               << " closed. " << endl; 
        if (Global::auto_reconnect) {
          push(new StartStopMessage(BGPstart,peer->nh));
        }
      }
      break;
      
    // - - - - - - - - - - - - - - OPENSENT - - - - - - - - - - - - - - - //
    case OPENSENT:
      switch (event_type) {
      case BGPstart: // ignored
        break;
      case TransConnClose:
        // 1. close transport connection
        // 2. restart ConnectRetry timer

        reset_timer(peer, BGP_Timer::CONNRETRY);
        peer->connection_state = ACTIVE;
        break;
      case TransFatalError:
        peer->close();
        peer->cancel_timers();
        peer->connection_state = IDLE;
        if (Global::DEBUG)
          cout << " BGP session with peer " <<  peer->ip_addr->val2str()
               << " closed. " << endl; 
        if (Global::auto_reconnect) {
          push(new StartStopMessage(BGPstart,peer->nh));
        }
        break;
      case RecvOpen:
        // 1. if process OPEN is OK
        //    - send KEEPALIVE message
        // 2. if process OPEN failed (this case never happens in simulation)
        //    - send NOTIFICATION message
        if (peer->ht != NULL) {
          // the Hold Timer may have been set (in the Active state)
          peer->ht->cancel();
        }          
        send(new KeepAliveMessage(nh), peer);

        // Determine negotiated Hold Timer interval (it is the minimum of the
        // value we advertised and the value that the (potential) peer
        // advertised to us.
        if (((OpenMessage*)msg)->hold_time < peer->hold_timer_interval) {
          peer->hold_timer_interval = ((OpenMessage*)msg)->hold_time;
        }
        
        // Set the Keep Alive Timer Interval for this peer based upon the
        // negotiated Hold Timer Interval for this peer, preserving the ratio
        // of the configured values for the two timer intervals, and adding
        // jitter.
        peer->keep_alive_interval = (int)(keep_alive_jitter *
                            peer->hold_timer_interval * peer->keephold_ratio);
        reset_timer(peer, BGP_Timer::HOLD);
        reset_timer(peer, BGP_Timer::KEEPALIVE);       
       

        if (peer->hold_timer_interval > 0) {
          if (peer->hold_timer_interval < 3.0) {
            // if the interval is not 0, then the minimum recommended
            // value is 3
            printf("non-zero Hold Timer value is less than the minimum recommended \
                    value 3s (current_val = %d s) \n",  peer->hold_timer_interval);
          }
        } else {
          cout << "hold timer value is 0 for peer " << peer->ip_addr->toString();
        }
        
	peer->bgp_id = new IPaddress(((OpenMessage*)msg)->bgpid);
	peer->bgp_id_ = peer->bgp_id->val();
        peer->connection_state = OPENCONFIRM;
                
        break;
      default: // for BGPstop, TransConnOpen, TransConnOpenFail,
               // ConnectRetryTimerExp, HoldTimerExp, KeepAliveTimerExp,
               // RecvKeepAlive, RecvUpdate, RecvNotification
        // 1. close transport connection
        // 2. release resources
        // 3. send NOTIFICATION message

        // (the two 0's in the line below should be changed to the
        // appropriate error code and subcode values, eventually ...)
        send(new NotificationMessage(nh, 0, 0), peer);
        if (peer->ht != NULL) {
          // the Hold Timer may have been set (in the Active or Connect state)
          peer->ht->cancel();
        }
        peer->close();
        peer->cancel_timers();
        peer->connection_state = IDLE;
        if (Global::DEBUG)
          cout << " BGP session with peer " <<  peer->ip_addr->val2str()
               << " closed. " << endl; 
        if (Global::auto_reconnect) {
          push(new StartStopMessage(BGPstart,peer->nh));
        }
      }
      break;
      
    // - - - - - - - - - - - - - - OPENCONFIRM - - - - - - - - - - - - - - //
    case OPENCONFIRM:
      switch (event_type) {
      case BGPstart: // ignored
        break;
      case TransConnClose:
      case TransFatalError:  // (same for both cases)
        // 1. release resources

        peer->close();
        peer->cancel_timers();
        peer->connection_state = IDLE;
        if (Global::auto_reconnect) {
          push(new StartStopMessage(BGPstart,peer->nh));
        }
        break;
      case KeepAliveTimerExp:
        // 1. restart KeepAlive timer
        // 2. resend KEEPALIVE message
        
        reset_timer(peer, BGP_Timer::KEEPALIVE);
        send(new KeepAliveMessage(nh), peer);
        break;
      case RecvKeepAlive: 
        // 1. complete initialization
        // 2. restart Hold Timer
        
        peer->inupdates = 0;
        peer->outupdates = 0;
        reset_timer(peer, BGP_Timer::HOLD);
        peer->connection_state = ESTABLISHED;
        if (Global::DEBUG)
          cout << " BGP session with peer " <<  peer->ip_addr->val2str()
               << " established. " << endl;
        // By running Phase 3 of the Decision Process, we advertise the local
        // address space to our new peer.
        locribchange.clear();
        decision_process_3(locribchange);
        break;
      case RecvNotification:
        // 1. close transport connection
        // 2. release resources
        // 3. send NOTIFICATION message

        // (the two 0's in the line below should be changed to the
        // appropriate error code and subcode values, eventually ...)
        send(new NotificationMessage(nh, 0, 0), peer);
        peer->close();
        peer->cancel_timers();
        peer->connection_state = IDLE;
        if (Global::DEBUG)
          cout << " BGP session with peer " <<  peer->ip_addr->val2str()
               << " closed. " << endl; 
        if (Global::auto_reconnect) {
          push(new StartStopMessage(BGPstart,peer->nh));
        }
        break;
      default:  // for BGPstop, TransConnOpen, TransConnOpenFail,
                // ConnRetryTimerExp, HoldTimerExp, RecvUpdate, RecvOpen
        // 1. close transport connection
        // 2. release resources
        // 3. send NOTIFICATION message

        // (the two 0's in the line below should be changed to the
        // appropriate error code and subcode values, eventually ...)
        send(new NotificationMessage(nh, 0, 0), peer);
        peer->close();
        peer->cancel_timers();
        peer->connection_state = IDLE;
        if (Global::DEBUG)
          cout << " BGP session with peer " <<  peer->ip_addr->val2str()
               << " closed. " << endl; 
        if (Global::auto_reconnect) {
          push(new StartStopMessage(BGPstart,peer->nh));
        }
      }
      break;
      
    // - - - - - - - - - - - - - - ESTABLISHED - - - - - - - - - - - - - - //
    case ESTABLISHED:
      switch (event_type) {
      case BGPstart: // ignored
        break;
      case TransConnClose:
      case TransFatalError: // (same for both cases)
        // 1. release resources
        printf("TransConnClose or TransFatalError occurred");
        peer->close();
        peer->cancel_timers();
        peer->connected = false;
        peer->connection_state = IDLE;
        remove_all_routes(peer);
        if (Global::auto_reconnect) {
          push(new StartStopMessage(BGPstart,peer->nh));
        }
        break;
      case KeepAliveTimerExp:
        // 1. restart KeepAlive timer
        // 2. send KEEPALIVE message
        
        reset_timer(peer, BGP_Timer::KEEPALIVE);
        send(new KeepAliveMessage(nh), peer);
        break;
      case RecvKeepAlive:
        // 1. restart Hold Timer
        reset_timer(peer, BGP_Timer::HOLD);
        break;
      case RecvUpdate:
        // 1. restart Hold Timer
        reset_timer(peer, BGP_Timer::HOLD);           
        handle_update((UpdateMessage*)msg);
        break;        
      case RecvNotification:
        // 1. close transport connection,
        // 2. release resources
        peer->close();
        peer->cancel_timers();
        peer->connected = false;
        peer->connection_state = IDLE;
        if (Global::DEBUG)
          cout << " BGP session with peer " <<  peer->ip_addr->val2str()
               << " closed. " << endl;                     
        remove_all_routes(peer);
        if (Global::auto_reconnect) {
         // push(new StartStopMessage(BGPstart,peer->nh));
          st = new BGP_Timer(this, BGPstart, peer);
          st->resched(Global::base_startup_wait);
        }
        break;
      default:  // for BGPstop, TransConnOpen, TransConnOpenFail,
                // ConnRetryTimerExp, HoldTimerExp, RecvOpen
        // 1. send NOTIFICATION message
        // 2. close transport connection
        // 3. release resources

        // (the two 0's in the line below should be changed to the
        // appropriate error code and subcode values, eventually ...)
        send(new NotificationMessage(nh, 0, 0), peer);
        peer->close();
        peer->cancel_timers();
        peer->connected = false;
        peer->connection_state = IDLE;
        if (Global::DEBUG)
          cout << " BGP session with peer " <<  peer->ip_addr->val2str()
               << " closed. " << endl; 
        remove_all_routes(peer);
        if (Global::auto_reconnect) {
          //push(new StartStopMessage(BGPstart,peer->nh));
          st = new BGP_Timer(this, BGPstart, peer);
          st->resched(Global::base_startup_wait);
        }
      }
      break;
    default:
      printf("unrecognized BGP state: %d\n" + peer->connection_state);
    }  
    return true; 
} // end of receive()
