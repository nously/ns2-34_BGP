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

#ifndef NEXTHOP_H
#define NEXTHOP_H

#include "attribute.h"
#include <string>
#include <typeinfo>
#include "../Util/ipaddress.h"

using namespace std;

/**
 * The next hop path attribute.  It describes the next hop in the path, which
 * is an IP address.  It is well-known and mandatory.
 */
class NextHop : public Attribute  {
public: 
	NextHop(IPaddress * ipa);
  NextHop(char * stream);
	virtual ~NextHop();   
  bool equals(Attribute * attrib); 
  int bytecount();      
  Attribute * copy();   
  string toString();
  void toStream(char * stream);
  
public:
  // ........................ member data .......................... // 
  /** The next hop IP address. */
  IPaddress * ipaddr;
  
  // ......................... constants ........................... // 
  /** The next hop path attribute type code. */
  const static int TYPECODE = 3;
};

#endif
