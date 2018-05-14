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

#ifndef classifier_ipv4_h
#define classifier_ipv4_h   

#include <iostream>
#include <string>
#include <map>
#include <sys/types.h>
#include "config.h"
#include "packet.h"
#include "ip.h"
#include "classifier.h" 
#include "tclcl.h" 
#include "Util/ipaddress.h"

using namespace std;

class ipv4Route {
  public:
    u_int32_t   network;
    u_int32_t   prefixlen;
    ipv4Route(u_int32_t n,u_int32_t p) { network = n; prefixlen = p; }
};
 
class less_Route {
  public:
    bool operator()(const ipv4Route& r1,const ipv4Route& r2) const {
 
      if ( r1.prefixlen > r2.prefixlen ) {
        return ( true );
      } else if ( r1.prefixlen == r2.prefixlen && r1.network < r2.network ) {
        return ( true );
      } else {
        return ( false );
      }
    }
};                  

class IPv4Classifier : public Classifier {
  friend class rtProtoBGP;     // in order to using the protected method classify(int).
  public:
	  IPv4Classifier();
    int add_route(u_int32_t net,u_int32_t prefix, const char* const node);
    int add_route(u_int32_t net,u_int32_t prefix, NsObject* node);
    void del_route(u_int32_t net,u_int32_t prefix);
	  void dumptable();
    int testclassify(const char*);
    u_int32_t strtoaddr(const char *);
    void  addrtostr(u_int32_t,char *);    
    
  protected:
	  int classify(Packet *p);
    int classify(u_int32_t daddr);
    void do_install(char * dst, NsObject *nullagent);
	  virtual int command(int,const char*const*);
    inline float now() {return Scheduler::instance().clock();}
    map<ipv4Route,int,less_Route> RouteTable;
	  int routecnt;
};

#endif
