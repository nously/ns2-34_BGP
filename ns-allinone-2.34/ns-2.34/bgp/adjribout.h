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

#ifndef ADJRIBOUT_H
#define ADJRIBOUT_H

#include "ribelement.h"

class PeerEntry;
class rtProtoBGP; 

/**
 * One element of the Adj-RIBs-Out section of BGP's Routing Information Base.
 */
class AdjRIBOut : public RIBElement  {
public: 
	AdjRIBOut( PeerEntry *pe, rtProtoBGP *b);
	~AdjRIBOut();   
  RouteInfo * add(RouteInfo * info);
  RouteInfo * replace(RouteInfo *info);
  RouteInfo * remove(IPaddress *ipa);
  
public:
  /** The peer with whom this RIB element is associated, if any. */
  PeerEntry * peer;
};

#endif
