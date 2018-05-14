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

#include "aspath.h"             

  // ----- constructor ASpath ---------------------------------------------- //
  /**
   * Constructs an AS path attribute given a list of path segments.
   *
   * @param sv  A vector of path segments.
   */
ASpath::ASpath(segVector sv) :Attribute(false,true,false){
  segs = sv;
}

  // ----- constructor ASpath ---------------------------------------------- //
  /**
   * Constructs an AS path attribute given a ASpath object.
   *
   * @param asp  A ASpath pointer.
   */
ASpath::ASpath(ASpath * asp) :Attribute(false,true,false){
  segs = asp->segs;
}

  // ----- constructor ASpath ---------------------------------------------- //
  /**
   * Constructs an AS path attribute given a char stream.
   *
   * @param stream  A char stream contains the vector of path segments.
   */
ASpath::ASpath(char * stream):Attribute(false,true,false){
   const int size = (int)stream[2];
   int k=3;
   char * seg_stream;  
   while (k<size) {
     int seg_size = 2+2*((int)stream[k+1]);
     seg_stream = new char[seg_size];
     for (int i=0;i<seg_size;i++,k++) {
         seg_stream[i]=stream[k];
     }
     Segment * seg =  new Segment(seg_stream);
     append_segment(seg);
  }
  delete[] seg_stream;
}

ASpath::~ASpath(){
  for (int i=0; i<segs.size(); i++) {
      if (segs[i])
         delete segs[i];
  }
}


  // ----- ASpath.copy ----------------------------------------------------- //
  /**
   * Constructs and returns a copy of the attribute.
   *
   * @return a copy of the attribute
   */
Attribute * ASpath::copy(){
  segVector sv;
    for (int i=0; i<segs.size(); i++) {
      sv.push_back(new Segment(*segs[i]));
    }
    return (new ASpath(sv));
}

  // ----- ASpath.length --------------------------------------------------- //
  /**
   * Returns the number of ASes in the path.
   *
   * @return the number of ASes in the path
   */
int ASpath::length(){

  if (segs.size() == 0) {
      return 0;
  } else {
      int len = 0;
      for (int i=0; i<segs.size(); i++) {
        len += (segs[i]->size());
      }
      return len;
  }
}

  // ----- ASpath.append_segment ------------------------------------------- //
  /**
   * Appends a path segment to the list of segments.
   *
   * @param ps  The path segment to append to the list of segments.
   */
void ASpath::append_segment(Segment * ps){
  segs.push_back(ps);
}

  // ----- ASpath.append_segment ------------------------------------------- //
  /**
   * Appends a path segment to the list of segments.
   *
   * @param ps  The path segment to append to the list of segments.
   */
void ASpath::prepend_as(string asnh){
      strVector tempsv;
      if (segs.size() == 0) {
        Segment * seg = new Segment(Segment::SEQ, tempsv);
        seg->prepend_as(asnh);
        segs.push_back(seg);
      } else {
        if (segs[0]->type == Segment::SEQ) {
        // insert the AS number at the beginning of the list
          segs[0]->prepend_as(asnh);
        } else {
        // create a new segment of type AS_SEQUENCE
          Segment * seg = new Segment(Segment::SEQ, tempsv);
          seg->append_as(asnh);
          segs.insert(segs.begin(),seg);
        }
      }
}

  // ----- ASpath.equals --------------------------------------------------- //
  /**
   * Determines whether or not this path attribute is equivalent to another.
   *
   * @param attrib  A path attribute to compare to this one.
   * @return true only if the two attributes are equivalent
   */
bool ASpath::equals(Attribute * attrib){
    if ( attrib == NULL &&
          !(typeid(*attrib) == typeid(ASpath)) )
      return false;

    if (segs.size() != ((ASpath*)attrib)->segs.size()) {
      return false;
    }
    for (int i=0; i<segs.size(); i++) {
      if ( !segs[i]->equals(((ASpath*)attrib)->segs[i]) ) {
        return false;
      }
    }
    return true;

}

  // ----- ASpath.bytecount ------------------------------------------------ //
  /**
   * Returns the number of octets (bytes) needed to represent this AS path
   * attribute in an update message.  The number is the sum of the two octets
   * needed for the attribute type (which contains attribute flags and the
   * attribute type code), the one or two octets needed for the attribute
   * length, and the variable number of octets needed for the attribute value.
   *
   * @return the number of octets (bytes) needed to represent this AS path
   *         attribute in an update message
   */
int ASpath::bytecount() {
    int octets = 2; // 2 octets for the attribute type
    for (int i=0; i<segs.size(); i++) {
      // 1 octet for the seg type, 1 for seg length, 2 per AS# in segment
      octets += 1 + 1 + 2*(segs[i]->size());
    }

    if (octets > 255) { // 1 or 2 octets for the attribute length field
      octets += 2;
    } else {
      octets++;
    }
    return octets;
}

  // ----- ASpath.contains ------------------------------------------------- //
  /**
   * Determines whether or not the path contains a given AS.
   *
   * @param asnh  The NHI prefix address of the AS to look for in the AS path.
   * @return true only if the AS was in the AS path
   */
bool ASpath::contains(string asnh){

     for (int i=0; i<segs.size(); i++) {
      if (segs[i]->contains(asnh)) {
        return true;
      }
    }
    return false;
}

  // ----- ASpath.toMinString(char) -------------------------------- //
  /**
   * Returns the AS path as a string, leaving out set/sequence information.
   *
   * @param sepchar  The character used to separate AS numbers in the list.
   * @return the AS path as a string, without set/sequence info
   */
string ASpath::toMinString(char sepchar) {
    string str = "";
    for (int i=0; i<segs.size(); i++) {
      if (i != 0) {
        str += sepchar;
      }
      str += segs[i]->toMinString(sepchar);
    }
    return str;
}

  // ----- ASpath.toMinString() -------------------------------------------- //
  /**
   * Returns the AS path as a string, leaving out set/sequence information.
   *
   * @return the AS path as a string, without set/sequence info
   */
string ASpath::toMinString() {
    return toMinString(' ');
}


  // ----- ASpath.toString ------------------------------------------------- //
  /**
   * Returns the AS path as a string.
   *
   * @return the AS path as a string
   */
string ASpath::toString() {
    string str = "";
    for (int i=0; i<segs.size(); i++) {
      if (i != 0) {
        str += " ";
      }
      str += segs[i]->toString();
    }

    return str;
}  

  // ----- ASpath.toStream ------------------------------------------------- //
  /**
   * Returns the AS path as a char stream.
   *
   * @return the AS path as a char stream pointed by the parameter stream.
   */
void ASpath::toStream(char * stream){
  char * c_flag= new char[1];
  Attribute::toStream(c_flag);
  stream[0] = c_flag[0];
  stream[1] = ASpath::TYPECODE;
  stream[2] = bytecount();
  delete[] c_flag;
  int k=3;
  const int size = segs.size();

  for (int i=0;i<size;i++) {
    char * s_stream = new char[segs[i]->bytecount()];
    segs[i]->toStream(s_stream);
    for (int j=0;j<segs[i]->bytecount();j++) {       
      stream[k]= s_stream[j];       
      k++;
    }      
   delete [] s_stream;
  }   
}  
