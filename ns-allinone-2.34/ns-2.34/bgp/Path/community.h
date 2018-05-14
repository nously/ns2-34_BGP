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

#ifndef COMMUNITY_H
#define COMMUNITY_H  

#include "attribute.h" 
#include <string>
#include <vector> 
#include <typeinfo>

using namespace std; 
typedef vector<string>  strVector; 

/** The community path attribute.  It is used to group routes together in order
  * to simplify the configuration of complex routing policies.  A route may be
  * a member of any number of communities.  The attribute is optional
  * non-transitive. 
  */
class Community : public Attribute  {
public: 
	Community(strVector sv);
	virtual ~Community(); 
  bool equals(Attribute * attrib);  
  int bytecount();  
  Attribute * copy();   
  string toString();
  
public:
  // ........................ member data .......................... // 
  /** The vector of community values. */
  strVector vals;
  
  // ......................... constants ........................... //
  /** The community path attribute type code. */ 
  const static int TYPECODE = 8;

};

#endif
