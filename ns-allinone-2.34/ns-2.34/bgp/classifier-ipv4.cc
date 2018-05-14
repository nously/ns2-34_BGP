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

#include "classifier-ipv4.h"    

IPv4Classifier::IPv4Classifier() { 
	routecnt = 0;
}

void
IPv4Classifier::dumptable() {  
  char buf[20];  // enough to hold a dotted quad   
  map<ipv4Route,int,less_Route>::iterator r;
  cout << "nlri" << "\t\t"  << "slot" << "\n";
  for ( r = RouteTable.begin(); r != RouteTable.end(); r++ ) {
    ipv4Route cr  = r->first;
    addrtostr(cr.network,buf);      
    cout << buf << "/" << cr.prefixlen << "\t"  << RouteTable[cr] <<  endl;
  }
}

void
IPv4Classifier::do_install(char * dst, NsObject *nullagent) {
          int net = atoi(dst);
          int pref = 32;
          int slot = add_route(net,pref,nullagent);
          install(slot,nullagent);
}          
  
int
IPv4Classifier::add_route(u_int32_t net,u_int32_t prefix, const char* const c_node) {
   Tcl& tcl = Tcl::instance();
   NsObject* node = (NsObject*)TclObject::lookup(c_node);
   if ( node == NULL ) {
        printf("classifier - install route null object (%s) specified as target\n",c_node);
   }
   return add_route(net,prefix,node);
}

int
IPv4Classifier::add_route(u_int32_t net,u_int32_t prefix, NsObject* node) {
   int slot = getnxt(node);
   ipv4Route r = ipv4Route(net,prefix);
   RouteTable[r] = slot;
   install(slot,node);
   return slot;
}   

void
IPv4Classifier::del_route(u_int32_t net,u_int32_t prefix) {
  ipv4Route r = ipv4Route(net,prefix);
  RouteTable.erase(r);
}

int
IPv4Classifier::classify(Packet *p) {
    hdr_ip* iph = HDR_IP(p);
    return classify(iph->daddr());
}

int
IPv4Classifier::classify(u_int32_t daddr) {
    map<ipv4Route,int,less_Route>::iterator r;

    for ( r = RouteTable.begin(); r != RouteTable.end(); r++ ) {
      ipv4Route cr = r -> first;
      if ( cr.prefixlen == 0 ) {
        return( RouteTable[cr] );
      }
      if ( (cr.network >> (32 - cr.prefixlen)) == ((u_int32_t)daddr >> (32 - cr.prefixlen)) ) {
          return( RouteTable[cr] );
      }
    }
    return( -1 );
}    

u_int32_t
IPv4Classifier::strtoaddr(const char *s) {
  
  int res = 0;
  string work = s;
  int place = work.find(".");
  res = atoi(work.substr(0,place).c_str());
  work = work.substr(place+1);
  place = work.find(".");
  res = ( res << 8 ) + atoi(work.substr(0,place).c_str());
  work = work.substr(place+1);
  place = work.find(".");
  res = ( res << 8 ) + atoi(work.substr(0,place).c_str());
  work = work.substr(place+1);
  place = work.find(".");
  res = ( res << 8 ) + atoi(work.substr(0,place).c_str());

  return(res);
}

void
IPv4Classifier::addrtostr(const u_int32_t addr,char * buf) {

  sprintf(buf,"%d.%d.%d.%d",(addr & 0xff000000) >> 24,
	 (addr & 0x00ff0000) >> 16,
	 (addr & 0x0000ff00) >> 8,
	 (addr & 0x0000ff));

}
int
IPv4Classifier::command(int argc, const char*const* argv) {
   Tcl& tcl = Tcl::instance();

  if ( argc == 2 ) {
	      if ( strcmp(argv[1],"dumproutes") == 0 ) {
           dumptable();
           return ( TCL_OK );
        }
      
  } else if ( argc == 3 ) {
        if ( strcmp(argv[1],"delroute") == 0 ) {
          int net = atoi(argv[2]);
          int pref = 32;
          del_route(net,pref);  
          return ( TCL_OK );
        }
        
  } else if ( argc == 4 ) {
        // addroute with integer ip address, no prefix length.
        if ( strcmp(argv[1],"addroute") == 0 ) {
          int net = atoi(argv[2]);
          int pref = 32;
          int slot = add_route(net,pref,argv[3]);
          tcl.resultf("%d",slot);
          return ( TCL_OK );
        }
        if ( strcmp(argv[1],"delroute") == 0 ) {
          int net = strtoaddr(argv[2]);
          int pref = atoi(argv[3]) ;
          del_route(net,pref);
          return ( TCL_OK );
        }      
        
  } else if ( argc == 5 ) {
        if ( strcmp(argv[1],"addroute") == 0 ) {
          int net = strtoaddr(argv[2]);
          int pref = atoi(argv[3]);
          int slot = add_route(net,pref,argv[4]);
          tcl.resultf("%d",slot);
          return ( TCL_OK );
        }
  } 
  return ( TCL_ERROR );
}

static class IPv4ClassifierClass : public TclClass {
public:
	IPv4ClassifierClass() : TclClass("Classifier/IPv4") {}
	TclObject *create(int,const char*const*) {
	  return(new IPv4Classifier());
  }
} class_ipv4_classifier;
