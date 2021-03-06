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

#ifndef MED_H
#define MED_H

#include "attribute.h"
#include <string>
#include <typeinfo>  

using namespace std;

/**
 * The multiple exit discriminator path attribute.  It is used to help
 * discriminate between multiple exit points to the same neighboring AS.  It is
 * optional and non-transitive.
 */
class MED : public Attribute  {
public: 
	MED(int v);
  MED(char * stream);
	virtual ~MED();
  bool equals(Attribute * attrib);
  int bytecount();
  Attribute * copy();
  string toString();
  void toStream(char * stream);
  
public:
  // ........................ member data .......................... // 
  /** The multiple exit discriminator value. */
  int val;
  
  // ......................... constants ........................... //
  /** The multiple exit discriminator path attribute type code. */
  const static int TYPECODE = 4;
 
};

#endif
