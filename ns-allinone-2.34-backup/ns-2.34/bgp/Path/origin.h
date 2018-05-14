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

#ifndef ORIGIN_H
#define ORIGIN_H

#include "attribute.h"
#include <string>
#include <stdio.h>
#include <typeinfo>

using namespace std;

class Origin : public Attribute  {
public: 
	Origin(int t);
  Origin(char  * stream);
	virtual ~Origin(); 
  bool equals(Attribute * attrib);
  int bytecount();
  Attribute * copy();
  string toString();   
  void toStream(char * stream);
  
public:
  // ........................ member data .......................... //
  /** The origin type value. */
  int type;
  
  // ......................... constants ........................... //
  /** The origin path attribute type code. */
  const static int TYPECODE = 1;
  /** Indicates that the path information was originated by an interior gateway
   *  protocol. */
  const static int IGP = 0;
  /** Indicates that the path information was originated by an exterior gateway
   *  protocol. */
  const static int EGP = 1; 
  /** Indicates that the path information was originated by some means other
   *  than an IGP or an EGP.  In other words, the origin information is
   *  incomplete. */
  const static int INC = 2;
};

#endif
