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
#include <stdio.h>
#include "segment.h"
#include <stdlib.h> // Added by Reza Sahraei

  // ----- constructor Segment(int,ArrayList) ------------------------------ //
  /**
   * Generic constructor for initializing member data.
   *
   * @param ty   The type of the path segment.
   * @param asn  The AS NHI prefix addresses making up the path segment.
   */
Segment::Segment(int ty, strVector asn) {
    type   = ty;
    asnhs = asn;
  
}    

  // ----- constructor Segment(Segment) ------------------------------------ //
  /**
   * Constructs a path segment based on another one.
   *
   * @param seg  The path segment on which to base a new one.
   */
Segment::Segment(const Segment & seg) {
    type = seg.type;
    for (int i=0; i<seg.asnhs.size(); i++) {
      asnhs.push_back(seg.asnhs[i]);
    }
}

  // ----- constructor Segment(Segment) ------------------------------------ //
  /**
   * Constructs a path segment based on a char stream.
   *
   * @param stream  The char stream on which to base a new one.
   */
Segment::Segment(char * stream) {
    type = (int) stream[0];
    int asnhs_size = (int) stream[1];
    int i_asnh;
    char * c_asnh = new char[6];
    for (int i=0;i<asnhs_size;i++) {
      i_asnh =((int) stream[2+i*2])*256 + (int) stream[3+i*2];
      sprintf(c_asnh,"%d",i_asnh);
      string s_asnh = c_asnh;
      append_as(s_asnh);      
    }
    delete[] c_asnh;
}

Segment::~Segment(){
}

 // ----- Segment.contains ------------------------------------------------ //
  /**
   * Determines whether or not this path segment contains a given AS.
   *
   * @param asnh  The NHI prefix of the AS to look for in this segment.
   * @return true only if the AS is in this segment
   */
bool Segment::contains(string asnh){

  for (int i=0; i<asnhs.size(); i++) {
      if (asnhs[i]==asnh) {
        return true;
      }
    }
    return false;
}   

  //--------------------------- Segment.prepend_as ------------------------- //
  /**
   * Adds an AS NHI prefix address to the beginning of the list.
   *
   * @param asnum  The AS NHI prefix address to prepend to this segment.
   */
void Segment::prepend_as(string asnh){
    asnhs.insert(asnhs.begin(), asnh);     
}

  //--------------------------- Segment.append_as -------------------------- //
  /**
   * Adds an AS NHI prefix address to the end of the list.
   *
   * @param asnh  The AS NHI prefix address to append to this segment.
   */
void Segment::append_as(string asnh){
    asnhs.push_back(asnh);
}

  // ----- Segment.size ---------------------------------------------------- //
  /**
   * Returns the number of ASes in this path segment.
   *
   * @return the number of ASes in this path segment
   */
int Segment::size(){
  return asnhs.size();
}

int Segment::bytecount(){
  // 1 octet for the seg type, 1 for seg length, 2 per AS# in segment
   return 1 + 1 + 2*size();  
}

// ----- Segment.equals -------------------------------------------------- //
  /**
   * Returns true only if the two path segments are equivalent.  This means
   * that if it is a sequence, the AS NHI prefix addresses must be in the same
   * order, but if it is a set, they need not be in the same order.
   *
   * @param seg  The path segment to compare with this one.
   * @return true if either of two cases holds: (1) the segments are
   *         both sequences and have identical lists of AS NHI prefix
   *         addresses or (2) the segments are both sets and contain
   *         exactly the same AS NHI prefix addresses, not necessarily
   *         in the same order
   */
bool  Segment::equals(Segment * seg) {
    if (type != seg->type) {
      return false;
    }
    if (asnhs.size() != seg->asnhs.size()) {
      return false;
    }
    if (type == SEQ) {
      for (int i=0; i<asnhs.size(); i++) {
        if (!(asnhs[i] == seg->asnhs[i])) {
          return false;
        }
      }
    } else {
      bool found;
      for (int i=0; i<asnhs.size(); i++) {
        found = false;
        for (int j=0; j<asnhs.size(); j++) {
          found = found || (asnhs[i] == seg->asnhs[j]);
        }
        if (!found) {
          return false;
        }
      }
    }
    return true;
}


  // ----- Segment.toMinString() ------------------------------------------- //
  /**
   * Returns this path segment as a string, leaving out set/sequence info.
   *
   * @return the path segment as a string of ASes
   */
string Segment::toMinString() {
    return toMinString(' ');
} 

  // ----- Segment.toMinString(char,boolean) ------------------------------- //
  /**
   * Returns this path segment as a string, leaving out set/sequence info.
   *
   * @param sepchar  The character used to separate ASes in the string.
   * @param usenhi   Whether to show ASes as NHI address prefixes or numbers
   * @return the path segment as a string of ASes
   */
string Segment::toMinString(char sepchar) {
    string str = "";
    for (int i=0; i<asnhs.size(); i++) {
      if (i != 0) {
        str += sepchar;
      }   
      str += asnhs[i];
    }
    return str;
}


  // ----- Segment.toString ------------------------------------------------ //
  /**
   * Returns this path segment as a string.
   *
   * @return the path segment as a string
   */
string Segment::toString() {
    string str = "";
    if (asnhs.size()==0) {
      return str;
    }
    if (type == SET) {
      str += "{"; // sets use curly braces
      for (int i=0; i<asnhs.size()-1; i++) {
        str += asnhs[i] + " ";
      }
      str += asnhs[asnhs.size()-1] + "}";
    } else { // SEQ
      str += "("; // sequences use parens
      for (int i=0; i<asnhs.size()-1; i++) {
        str += asnhs[i] + " ";
      }
      str += asnhs[asnhs.size()-1] + ")";
    }
     return str;
 }

  // ----- Segment.toString ------------------------------------------------ //
  /**
   * Returns this path segment as a char stream.
   *
   * @return the path segment as a char stream
   */
void Segment::toStream(char * stream){
  stream[0] = type;
  stream[1] = asnhs.size();   
  for (int i=0;i<asnhs.size();i++) {
    stream[2+i*2] = atoi(asnhs[i].c_str()) / 256;
    stream[3+i*2] = atoi(asnhs[i].c_str()) % 256; 
  }
}
