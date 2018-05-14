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

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <string>
using namespace std;

class Attribute {
public:
  Attribute();
	Attribute(bool o,bool t,bool p);
	virtual ~Attribute();

  virtual int bytecount();
  virtual string toString();
  virtual bool equals (Attribute * attrib){};  
  virtual Attribute * copy(){};
  virtual void toStream(char * stream );
  
public:
  // ......................... constants ........................... //
  /** The lowest path attribute type code value. */
  const static int MIN_TYPECODE = 1;
  /** The highest path attribute type code value used in the simulation.  The
   *  actual maximum typecode used in practice may be higher. */
  const static int MAX_TYPECODE = 10;
  
  // ........................ member data .......................... //
  /** Defines whether the path attribute is optional (true) or well-known
   *  (false). */
  bool opt;
  
  /** Defines whether an optional attribute is transitive (true) or
   *  non-transitive (false).  For well-known attributes it must be true. */
  bool trans;
  
  /** Defines whether or not the information contained in the optional
   *  transitive attribute is partial (true) or complete (false).  For
   *  well-known attributes and for optional non-transitive attributes, it must
   *  be false. */
  bool partial;
};

#endif
