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


#ifndef ROUTEINFO_H
#define ROUTEINFO_H

#include "route.h" 
#include "Util/stringmanip.h"
#include <stdlib.h>
#include <iostream> // Changed by Reza Sahraei

class rtProtoBGP;
class PeerEntry;

/**
 * A unit of BGP route information.  It contains all information about a
 * particular route which is used by BGP, including the destination, path
 * attributes, and degree of preference.
 */
class RouteInfo {
public:
	RouteInfo(rtProtoBGP *b, Route *r, int aggs, Route *aggr, int d,
                  bool feas, bool perm, bool isn, PeerEntry *pe);
  RouteInfo(rtProtoBGP *b);
  RouteInfo(rtProtoBGP *b, RouteInfo *info);
  RouteInfo(rtProtoBGP *b, Route *rte, int dop, bool feas, PeerEntry *pe);
  RouteInfo(){};
	~RouteInfo();
  void set_permissibility(bool perm);
  int compare(RouteInfo *info);
  string toString();

public:
  /** Indicates that this route is not part of any aggregated route. */
  const static int AGG_NONE = 0;  
  /** Indicates that this route is part of an aggregation in which there is a
   *  less specific route than this one. */
  const static int AGG_CONTAINED = 1;    
  /** Indicates that this route is part of an aggregation in which this route
   *  is the least specific. */
  const static int AGG_CONTAINS = 2;
  /** The minimum value for degree of preference. */
  const static int MIN_DOP = 0;     
  /** The maximum value for degree of preference. */
  const static int MAX_DOP = 100;

  // ........................ member data .......................... //

  /** The route itself. */
  Route * route;

  /** The status of this information with regard to route aggregation (see
   *  descriptions of the three possible states). */
  int agg_status;

  /** An aggregation of this route with others, if appropriate.  Used during
   *  Phase 3 of the Decision Process. */
  Route * agg_route;

  /** The degree of preference of this route (according to BGP policy). */
  int dop;

  /** Whether or not the route is feasible.  A feasible route is one that could
   *  be used, according to BGP specifications.  That is, it does not contain
   *  any AS loops have any other "illegal" properties. */
  bool feasible;

  /** Whether or not the route is permissible.  A permissible route is one
   *  which was permitted by the input policy rules.  Permissibility is
   *  orthogonal to feasibility. */
  bool permissible;

  /** Whether this route is newly installed or not.  It is no longer new if it
   *  has been advertised to peers, or if it need not be advertised. */
  bool isnew;

  /** Whether or not this route is in the Loc-RIB. */
  bool inlocrib;

  /** The peer who advertised this route. */
  PeerEntry * peer;

  /** The rtProtoBGP with which this route info is associated. */
  rtProtoBGP * bgp;

  /** The utility class useful for manipulating character strings. */
  StringManip * sManip;

}; 

#endif
