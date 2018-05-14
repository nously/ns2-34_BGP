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
 * Authors: Rob Ballantyne <ballanty@cs.sfu.ca>, Tony Dongliang Feng
 * <tdfeng@cs.sfu.ca>.
 *
 */

#include "classifier-ipv4src.h"     

int
IPv4SrcClassifier::classify(Packet *p) {

    hdr_ip* iph = HDR_IP(p);
    map<ipv4Route,int,less_Route>::iterator r;

    for ( r = RouteTable.begin(); r != RouteTable.end(); r++ ) {
      ipv4Route cr = r -> first;
      if ( cr.prefixlen == 0 ) {
         return( RouteTable[cr] );
      }
     if ( (cr.network >> (32 - cr.prefixlen)) ==
          ((u_int32_t)iph->saddr() >> (32 - cr.prefixlen)) ) {
        return( RouteTable[cr] );
      }
    }
    // Return slot# of route 0/32 as default.
    // Slot of route 0/32 connect to the listening tcp-agent.
    ipv4Route dr = ipv4Route(0,32);
    return(RouteTable[dr] );
}       

static class IPv4SrcClassifierClass : public TclClass {
public:
	IPv4SrcClassifierClass() : TclClass("Classifier/IPv4/Src") {}
	TclObject *create(int,const char*const*) {
	  return(new IPv4SrcClassifier());
        }
} class_ipv4src_classifier;
