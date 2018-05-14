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


#ifndef OPENMESSAGE_H
#define OPENMESSAGE_H

#include "bgpmessage.h"
#include "../Util/ipaddress.h"

/**
 * A BGP Open message.  Used to initiate negotiation of a peering session
 * with a neighboring BGP speaker.
 */
class OpenMessage : public BGPMessage  {
public: 
	OpenMessage(IPaddress *bgp_id, string bgp_as, string nh, long ht);
  OpenMessage(unsigned char * stream, string nh);
  string toString();
  int body_bytecount();
  void toStream(char * stream);
	virtual ~OpenMessage();
  
public:
  /** The NHI prefix address of the autonomous system of the sender. */
  string as_nh;
  
  /** The length of time (in logical clock ticks) that the sender proposes for
   *  the value of the Hold Timer.  */
  long hold_time;
   
  /** The BGP Identifier of the sender.  Each BGP speaker (router running BGP)
   *  has a BGP Identifier.  A given BGP speaker sets the value of its BGP
   *  Identifier to an IP address assigned to that BGP speaker (it's the IP
   *  address of this router for now).  It is chosen at startup
   *  and never changes. */
  IPaddress *bgpid;
};

#endif
