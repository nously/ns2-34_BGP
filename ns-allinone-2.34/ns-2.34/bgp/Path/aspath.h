/***************************************************************************
                          aspath.h  -  description
                             -------------------
    begin                : Sun May 18 2003
    copyright            : (C) 2003 by 
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ASPATH_H
#define ASPATH_H

#include "attribute.h"
#include <vector>
#include "segment.h"  
#include <typeinfo>
#include <iostream> // Changed by Reza Sahraei

using namespace std;      
typedef vector<Segment*>  segVector;

/**
 * The AS path attribute.  An AS path is composed of a sequence of AS path
 * segments, where each segment is either an unordered set of AS numbers or an
 * ordered sequence of AS numbers.  It is well-known and mandatory.
 */
class ASpath : public Attribute  {
public: 
	ASpath(segVector sv);
  ASpath(ASpath * asp);
  ASpath(char * stream);
	virtual ~ASpath();      
  int length();  
  void append_segment(Segment *ps); 
  void prepend_as(string asnh);  
  bool equals(Attribute * attrib);   
  int bytecount();           
  bool contains(string asnh);        
  string toMinString(char sepchar);    
  string toMinString();      
  string toString();         
  void toStream(char * stream);
  Attribute * copy();
  
public:
  // ........................ member data .......................... //  
  /** An ordered vector of AS path segments. */
  segVector segs;
  
  // ......................... constants ........................... //    
  /** The AS path attribute type code. */
  const static int TYPECODE = 2;    
};

#endif
