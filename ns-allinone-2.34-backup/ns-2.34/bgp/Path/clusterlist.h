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

#ifndef CLUSTERLIST_H
#define CLUSTERLIST_H

#include "attribute.h" 
#include <vector> 
#include <string>
#include <typeinfo> 

using namespace std;
typedef vector<int>  intVector;

/**
 * The cluster list path attribute.  It is used to record the path that a route
 * has taken through the route reflection hierarchy.  It is optional and
 * non-transitive.
 */
class ClusterList : public Attribute  {
public: 
	ClusterList(intVector l);
  ClusterList(char * stream);
	virtual ~ClusterList();   
  int length();   
  void append(int cnum);    
  bool contains(int cnum);  
  bool equals(Attribute *attrib); 
  int bytecount();       
  string toString();
  void toStream(char * stream);   
  Attribute * copy();
  
public:
  // ........................ member data .......................... // 
  /** The vector of cluster numbers which represents the path that a route has
   *  taken through the route reflection hierarchy. */
  intVector list;
  
  // ......................... constants ........................... // 
  /** The cluster list path attribute type code. */
  const static int TYPECODE = 10;

};

#endif
