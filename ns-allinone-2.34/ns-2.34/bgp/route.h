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


#ifndef ROUTE_H
#define ROUTE_H
#include <iostream> // Changed by Reza Sahraei
#include "Path/attribute.h"
#include "Path/aspath.h"
#include "Path/community.h"
#include "Path/localpref.h"
#include "Path/segment.h"
#include "Path/med.h"
#include "Path/nexthop.h"
#include "Path/origin.h"
#include "Path/aggregator.h"
#include "Path/clusterlist.h"
#include "Path/originatorid.h"
#include "Path/atomicaggregate.h"     

/**
 * Information describing a single route.  A route is described as a unit of
 * information that pairs a destination with the attributes of a path to that
 * destination.  In other words, route = destination + path attributes.
 */
class Route {
public: 
	Route();
  Route(IPaddress * ipa, Attribute ** a);
  Route(Route * rte);
  Route(char * stream, IPaddress * rt);
	~Route();

  Attribute ** copy_attribs(Attribute ** pas);
  Aggregator * aggregator(); 
  ASpath * aspath();    
  int localpref();                   
  ClusterList * cluster_list();
  int med();                         
  int origin();                      
  IPaddress * orig_id();             
  IPaddress * nexthop();                    
  void prepend_as(string asnh);      
  void append_cluster(int cnum);
  bool has_aspath();
  bool has_med();     
  bool has_atomicagg();              
  bool has_aggregator();             
  bool has_cluster_list();           
  bool has_localpref();              
  bool has_orig_id();                
  void remove_attrib(int type);      
  void set_atomicagg() ;             
  void set_aggregator(string nh, IPaddress * ipa);
  void set_localpref(int pref);                   
  void set_med(int v);                            
  void set_origin(int type);                      
  void set_orig_id(IPaddress *  orig_id);         
  void set_nexthop(IPaddress * nexthop);
  void set_nlri(IPaddress * ipaddr); 
  int pas_bytecount();           
  void incorporate_route(Route * r);              
  bool equals(Route *rte);                        
  bool equal_attribs(Attribute ** attribs);       
  void pasToStream(char * stream);                 
  void nlriToStream(char * stream);                 
  string toString();
  
public:
  // ........................ member data .......................... //
  
  /** The destination of the route.  This is actually just an IP address
   *  prefix, so it may represent a specific host, subnet, AS, or perhaps an
   *  even larger (possibly geographical) portion of the network. */
   
  IPaddress * nlri;
  
  /** An array of path attributes which describe the path to the destination.
   *  This information is received via update messages. */
  Attribute ** pas;
  
  // ......................... constants ........................... //
  
  /** A route attribute "type code" value for NLRI.  It is negative since the
   * type codes for path attributes are all positive. */
  const static int NLRI = -1;
};

#endif
