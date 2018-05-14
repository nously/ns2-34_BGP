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

#ifndef ORIGINATORID_H
#define ORIGINATORID_H

#include "attribute.h"
#include <string>
#include <stdio.h>
#include <typeinfo>
#include "../Util/ipaddress.h"

using namespace std;

/**
 * The originator ID path attribute.  It records the IP address identifier of
 * the router that originated the route into the IBGP mesh.  It is optional and
 * non-transitive.
 */
class OriginatorID : public Attribute  {
public: 
	OriginatorID(IPaddress * ipa);
  OriginatorID(char * stream);
	virtual ~OriginatorID();
  bool equals(Attribute *attrib);
  int bytecount();
  Attribute * copy();
  string toString();
  void toStream(char * stream);
  
public:
  // ........................ member data .......................... //
  /** The ID of the originating router. */
  IPaddress * id;
  
  // ......................... constants ........................... // 
  /** The originator ID path attribute type code. */
  const static int TYPECODE = 9; 
};

#endif
