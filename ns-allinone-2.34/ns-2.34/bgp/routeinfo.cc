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


#include "routeinfo.h"
#include "rtProtoBGP.h"
#include "peer-entry.h"               

  // ----- constructor RouteInfo(Route,int,Route,int,boolean, -------------- //
  // -----                       boolean,boolean,PeerEntry)   -------------- //
  /**
   * Constructs route information given all of the relevant data.
   *
   * @param b     The rtProtoBGP with which this route info is associated.
   * @param r     The route to which this information pertains.
   * @param aggs  The aggregation status of this information.
   * @param aggr  An aggregation of this route with others (may be null).
   * @param d     The degree of preference of the route.
   * @param feas  Whether or not the route is feasible.
   * @param perm  Whether or not the route is permissible.
   * @param isn   Whether or not the route is marked as new.
   * @param pe    The peer who advertised this route.
   */                                                    
RouteInfo::RouteInfo(rtProtoBGP *b, Route *r, int aggs, Route *aggr, int d,
                   bool feas, bool perm, bool isn, PeerEntry *pe) {
    bgp         = b;
    route       = r;
    agg_status  = aggs;
    agg_route   = aggr;
    dop         = d;
    feasible    = feas;
    permissible = perm;
    isnew       = isn;
    peer        = pe;
    inlocrib    = false;
}       

  // ----- constructor RouteInfo() ----------------------------------------- //
  /**
   * Constructs new route information with default values.
   *
   * @param b  The rtProtoBGP with which this route info is associated.
   */
RouteInfo::RouteInfo(rtProtoBGP *b) {
    RouteInfo::RouteInfo(b, NULL, AGG_NONE, NULL, MIN_DOP, false, false, true, NULL);
}

  // ----- constructor RouteInfo(RouteInfo) -------------------------------- //
  /**
   * Constructs new route information from the given route information.
   *
   * @param b     The rtProtoBGP with which this route info is associated.
   * @param info  The route information to use for constructing this new one.
   */
RouteInfo::RouteInfo(rtProtoBGP *b, RouteInfo *info) {
    RouteInfo::RouteInfo(b, new Route(info->route), info->agg_status,
         (info->agg_route==NULL)?NULL:new Route(info->agg_route), info->dop,
         info->feasible, info->permissible, info->isnew, info->peer);
}

  // ----- constructor RouteInfo(Route,int,boolean,PeerEntry) -------------- //
  /**
   * Constructs new route information with the given attributes.
   *
   * @param b     The rtProtoBGP with which this route info is associated.
   * @param rte   The route held by this entry.
   * @param dop   The degree of preference of the route.
   * @param feas  Whether or not the route is feasible.
   * @param pe    The entry for the peer who advertised this route.
   */
RouteInfo::RouteInfo(rtProtoBGP *b, Route *rte, int d, bool feas,
                   PeerEntry *pe) { 
    bgp         = b;
    route       = rte;
    agg_status  = AGG_NONE;
    agg_route   = NULL;
    dop         = d;
    feasible    = feas;
    permissible = false;
    isnew       = true;
    peer        = pe;
    inlocrib    = false;
}             
  
RouteInfo::~RouteInfo() {}

  // ----- RouteInfo::set_permissibility ------------------------------------ //
  /**
   * Sets the permissibility of the route in this entry.
   *
   * @param perm  Whether or not the route is permissible.
   */
void RouteInfo::set_permissibility(bool perm) {
    permissible = perm;    
}

  // ----- RouteInfo::compare ----------------------------------------------- //
  /**
   * Compares to route information for another route to determine one is more
   * preferable.
   *
   * @param rte  The route information to compare to.
   * @return 1 if this route is preferred, 0 if they are identically
   *         preferable, and -1 if the given route is preferred.
   */
int RouteInfo::compare(RouteInfo *info){
    if (info == NULL) {
      return 1;
    }

    if (dop < info->dop) {
      return -1;
    } else if (dop > info->dop) {
      return 1;
    }

    // Their degrees of preference are equal.  If both routes were received
    // from BGP speakers in the same AS, then the first tiebreaker uses the
    // MULTI_EXIT_DISC path attribute.  If not, we skip to the next tiebreaker.
    if (peer->as_num == info->peer->as_num) {
      // Having a MED is better than not.  (See 9.1.2.1, where it says that the
      // highest value should be assumed when MED is not presest.  Since lower
      // is better for MEDs, no MED is the worst possible.)
      if (route->has_med() && !info->route->has_med()) {
        return 1;
      } else if (!route->has_med() && info->route->has_med()) {
        return -1;
      }
      if (route->has_med() && info->route->has_med()) {
        if (route->med() < info->route->med()) {
          return 1;
        } else if (route->med() > info->route->med()) {
          return -1;
        }
      }
    }

    // Their MULTI_EXIT_DISC values are the same (or both routes were
    // not received from BGP speakers in the same AS), so go to the
    // next tiebreaker, which is based on cost (interior distance).
    // here we're supposed to compare interior distance/cost, but
    // that would seem to imply that forwarding tables could be
    // inconsistent across BGP speakers within this same AS, so
    // we'll forego this comparison until I understand it correctly  

    // (This next part (comparing the sources of the routes) is apparently not
    // used for internal tie-breakers (section 9.2.1.1).  Note, however, that
    // it is used during route selection in Phase 2 of the Decision process
    // (section 9.1.2.1).

    // Their costs are the same, go to the next tiebreaker, which is
    // the source of the route (External or Internal BGP peer)
    if (peer->typ == PeerEntry::INTERNAL &&
        info->peer->typ == PeerEntry::EXTERNAL) {
      return -1;
    } else if (peer->typ == PeerEntry::EXTERNAL &&
               info->peer->typ == PeerEntry::INTERNAL) {
      return 1;
    }

    // Their sources are the same, go to next tiebreaker, which is lowest BGP
    // ID of the BGP speakers that advertised them. 
    if (peer->bgp_id_ < info->peer->bgp_id_) {         
        return 1;
    } else if (peer->bgp_id_ > info->peer->bgp_id_) {
        return -1;
    }
      
    // They're exactly tied all the way around.
    return 0;
}

  // ------- RouteInfo::toString() ------------------------------------------ //
  /**
   * Returns route information as a string.
   *
   * @return the route information as a string
   */
string RouteInfo::toString() {

    StringManip * sManip = new StringManip();
    string str = "";
    string temp;
    char * buffer;

    // ---- status codes ----
    string feas = "*", best = ">";
    if (!feasible) { feas = " "; }
    if (!inlocrib) { best = " "; }
    str += feas + best + "   ";
    
    // ---- network ----
    str += sManip->pad(route->nlri->toString(), 19, ' ', true);

    // ---- next hop ----
    if (peer == peer->bgp->self) {
      str += "self              ";
    } else {
      if (route->nexthop()!= NULL) {
        temp = route->nexthop()->toString(); 
      }
      else
        temp = " ";
      str += sManip->pad(temp, 18, ' ', true);
    }
    
    // ---- metric ----
    str += "    -";

    // ---- local pref ----
    if (route->has_localpref()) {
       sprintf (buffer," %d",route->localpref());
       temp = string(buffer);
      str += sManip->pad(temp, 7, ' ', false);
    } else {
      str += "      -";
    }
    
    // ---- weight ----
    str += "      -";

    // ---- AS path ----
    if (route->has_aspath())
      str += " " + sManip->pad(route->aspath()->toMinString(' '),
                                 9, ' ', true);

    // ---- internal ----
    if (peer->typ == PeerEntry::INTERNAL) {
      str += " i";
    }
    str += "\n";
    return str;
}
