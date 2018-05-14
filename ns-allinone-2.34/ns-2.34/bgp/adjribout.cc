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

#include "adjribout.h"

  // ----- constructor AdjRIBOut ------------------------------------------- //
  /**
   * Constructs an element of Adj-RIBs-Out with a reference to the local BGP
   * protocol session and the peer associated with it.
   *
   * @param b   The local BGP protocol session.
   * @param pe  The peer with which this RIB is associated.
   */
AdjRIBOut::AdjRIBOut(PeerEntry *pe, rtProtoBGP *b):RIBElement(b){
  peer = pe;
}

AdjRIBOut::~AdjRIBOut() {}

  // ----- AdjRIBOut::add --------------------------------------------------- //
  /**
   * Adds route information.  If route information with the same NLRI already
   * exists, then the add fails and the pre-existing information is returned.
   *
   * @param info  The route information to add.
   * @return pre-existing route information, if any
   */
RouteInfo * AdjRIBOut::add(RouteInfo * info) {
    RouteInfo * ri = RIBElement::add(info);
    if (ri != NULL) {
       printf( "adding routing to AdjRIBOut failed! \n");
    }
    return ri;
}

  // ----- AdjRIBOut.replace ----------------------------------------------- //
  /**
   * Adds route information, replacing any pre-existing information with the
   * same NLRI.
   *
   * @param info  The route information to add.
   * @return the pre-existing route information, or null if there was none
   */
RouteInfo * AdjRIBOut::replace(RouteInfo *info) {
    RouteInfo *ri = RIBElement::replace(info); 
    return ri;
}

  // ----- AdjRIBOut.remove ------------------------------------------------ //
  /**
   * Removes the route information corresponding to the given route
   * destination.
   *
   * @param ipa  The destination of the route information to remove.
   * @return the removed route information
   */
RouteInfo * AdjRIBOut::remove(IPaddress * ipa) {
    RouteInfo * ri = RIBElement::remove(ipa);
    // if (ri == NULL) {
    //   printf( "The route being removed is not in AdjRIBOut! \n");
    // }
    return ri;
}

