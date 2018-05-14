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

#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include "attribute.h"
#include <string>    
#include <typeinfo>
#include "../Util/ipaddress.h"

using namespace std;

/**
 * The aggregator path attribute.  It can only be used with routes which are
 * aggregates, and it indicates the AS number and IP address of the BGP speaker
 * that formed the aggregate route.  It is optional and transitive.
 */
class Aggregator : public Attribute  {
public: 
	Aggregator(string nh, IPaddress * ipa);
  Aggregator(Aggregator * a );
  Aggregator(char * stream);
  virtual ~Aggregator(); 
  bool equals(Attribute *attrib);
  int bytecount();
  Attribute * copy();
  string toString();
  void toStream(char * stream);
  
public:
  // ........................ member data .......................... //
  /** The address prefix of the AS of the BGP speaker that formed the
   *  aggregate route. */
  string asnh;
  
  /** The IP address of the BGP speaker that formed the aggregate route. */
  IPaddress * ipaddr;
  
  // ......................... constants ........................... //
  /** The aggregator path attribute type code. */
  const static int TYPECODE = 7;
};

#endif
