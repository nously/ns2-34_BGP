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


#ifndef UPDATEMESSAGE_H
#define UPDATEMESSAGE_H

#include "bgpmessage.h"
#include <vector>
#include "../route.h"
#include "../Util/ipaddress.h"

/**
 * A BGP Update message.  An update message is used to transfer routing
 * information between two BGP peers.  That routing information consists of new
 * and/or outdated routes.  New routes are specified by a destination IP prefix
 * and path attributes which describe the route to that destination.  Outdated
 * routes, known as infeasible routes, are routes which are no longer valid.
 * They are indicated only by destination, and are used to inform a peer that
 * routes to that destination (as learned from the sending BGP speaker) are no
 * longer valid.
 */
class UpdateMessage : public BGPMessage  {
public: 
  UpdateMessage(string nh);
  UpdateMessage(string nh, Route *rte);
  UpdateMessage(string nh, IPaddress *wdnlri);
  UpdateMessage(unsigned char* stream,string nh);     
  virtual ~UpdateMessage();
  
  Route * rte(int ind);
  void add_route(Route * rte);
  IPaddress * wd(int ind);
  int body_bytecount();
  void add_wd(IPaddress *wd);
  bool remove_wd(IPaddress * ipa);
  bool is_empty();
  string toString();
  void toStream(char * stream);

public:
  // ........................ member data .......................... //

  /** A vector of the destinations of withdrawn routes.  Each element is an IP
   *  address prefix indicating a route which is no longer being used by the
   *  sending BGP speaker. */
  vector<IPaddress *> wds;

  /** A vector of routes being advertised.  Each element includes the NLRI and
   *  path attributes for the route. */
  vector<Route *> rtes;  

};

#endif
