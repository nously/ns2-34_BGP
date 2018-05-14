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

#ifndef SEGMENT_H
#define SEGMENT_H

#include <string>
#include <vector>
#include <iostream> // Changed by Reza Sahraei

using namespace std; 
typedef vector<string>  strVector;

/**
 * An AS path segment.  A path segment is a grouping of ASes (indicated by NHI
 * address prefix) which comprises a portion of an AS path.  A segment can
 * either be an unordered set of ASes or an ordered sequence of ASes.
 */ 
class Segment {
public:   
	Segment(int ty, strVector asn);
  Segment(const Segment & seg);
  Segment(char * stream);
	~Segment();
  bool contains(string asnh); 
  void append_as(string asnh);
  void prepend_as(string asnh); 
  int size();
  int bytecount(); 
  bool equals(Segment *seg); 
  string toMinString();
  string toMinString(char sepchar);
  string toString();     
  void toStream(char * stream);    
  
public:
  // ........................ member data .......................... // 
  /** The type of the path segment.  Either an unordered (set) or ordered
   *  (sequence) group of ASes. */
  int type;
  
  /** The AS NHI prefix addresses which make up this segment of the path. */
  strVector asnhs;

  // ......................... constants ........................... // 
  /** Indicates a segment type of AS_SET, meaning that the ASes in the segment
   *  are not ordered. */
  const static int SET = 1;
  /** Indicates a segment type of AS_SEQUENCE, meaning that the ASes in the
   *  segment are ordered.  Those closer to the beginning of the list (lower
   *  indices) have been added more recently. */
  const static int SEQ = 2;
};

#endif
