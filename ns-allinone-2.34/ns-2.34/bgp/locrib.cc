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
 

#include "locrib.h"
#include "rtProtoBGP.h"          

  // ----- constructor LocRIB ---------------------------------------------- //
  /**
   * Constructs the Loc-RIB with a pointer to the local BGP protocol agent.
   *
   * @param b  The local BGP protocal agent.
   */
LocRIB::LocRIB(rtProtoBGP *b):RIBElement(b) {}

LocRIB::~LocRIB() {}

  // ----- LocRIB::add ------------------------------------------------------ //
  /**
   * Adds route information.  If route information with the same NLRI already
   * exists, then the add fails and the pre-existing information is returned.
   *
   * @param info      The route information to add.
   * @return pre-existing route information, if any
   */
RouteInfo * LocRIB::add(RouteInfo * info) {
    RouteInfo * oldinfo = RIBElement::add(info);
    if (oldinfo == NULL) {
      info->inlocrib = true;
      bgp->ftadd(info);
    } else {
      printf( "adding routing to LocRIB failed! \n");
    }
    return oldinfo;
}

  // ----- LocRIB::replace -------------------------------------------------- //
  /**
   * Adds route information, replacing any pre-existing information with the
   * same NLRI.  Also performs the appropriate actions on the local forwarding
   * table.
   *
   * @param info  The route information to add.
   * @return the pre-existing route information, or null if there was none
   */
RouteInfo * LocRIB::replace(RouteInfo *info) {
    RouteInfo *oldinfo = RIBElement::replace(info);
    info->inlocrib = true;
    if (oldinfo != NULL) {
      bgp->ftrmv(oldinfo);           
    }
    bgp->ftadd(info);
    version++;
    return oldinfo;
}

  // ----- LocRIB::remove --------------------------------------------------- //
  /**
   * Removes the route information corresponding to the given route
   * destination from both this Loc-RIB and the local forwarding table.
   *
   * @param ipa  The destination of the route information to remove.
   * @return the removed route information
   */
RouteInfo * LocRIB::remove(IPaddress * ipa) {
    RouteInfo * oldinfo = RIBElement::remove(ipa);
    if (oldinfo != NULL) {
      oldinfo->inlocrib = false;
      bgp->ftrmv(oldinfo);
    }
    return oldinfo;
}

  // ----- LocRIB.remove_all ----------------------------------------------- //
  /**
   * Removes all route information from the Loc-RIB element, as well as from
   * the local forwarding table, and returns it as a list.
   *
   * @return a list of removed route information
   */
vector<RouteInfo*> LocRIB::remove_all() {
    vector<RouteInfo*> allrtes;
    RouteInfo * info;
    map<unsigned int,RouteInfo*>::iterator c = rtes.begin();
    while ( c!= rtes.end()) {
      if (((*c).second) != NULL) {
        info = remove((*c).second->route->nlri);
        allrtes.push_back(info);
       } 
      c++;
    }
    return allrtes;
}

