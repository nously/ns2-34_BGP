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

#ifndef RIBELEMENT_H
#define RIBELEMENT_H

#include "Util/radixtree.h"
#include "routeinfo.h"
#include <vector>
#include <map>    

/**
 * An element of a RIB.  A single RIBElement is either Loc-RIB, or part of
 * Adj-RIBs-In or Adj-RIBs-Out.  An array of RIBElements make up Adj-RIBs-In,
 * and the same is true for Adj-RIBs-Out (since they each have one element per
 * peer).
 */

class rtProtoBGP; 
 
class RIBElement : public RadixTree<RouteInfo>{
public:
	RIBElement(rtProtoBGP *b);
	~RIBElement();
  RouteInfo * add(RouteInfo *info);
  RouteInfo * replace(RouteInfo * info);
  RouteInfo * remove(IPaddress * ipa);
  vector<RouteInfo *> remove_all();
  vector<RouteInfo *> get_all_routes();
  RouteInfo *  find(IPaddress * ipa);
  vector<RouteInfo*> get_less_specifics(IPaddress *ipa);
  vector<RouteInfo*> get_more_specifics(IPaddress * ipa);
  bool is_less_specific(IPaddress * ipa);
  string toString();
  string toString_helper(RadixTreeNode<RouteInfo> * node);

public:
  // ........................ member data .......................... //  

  /** A table mapping NLRI to routes.  It adds no more functionality than the
   *  radix tree already had, but can save time during look-ups. */
  map<unsigned int,RouteInfo*> rtes;

   /** The local BGP protocol agent. */
  rtProtoBGP * bgp;
  
  /** The version number of the table.  Inspired by Cisco table version
   *  numbering. */
  int version;  
};

#endif
