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
#include "clusterlist.h"

  // ----- constructor ClusterList ----------------------------------------- //
  /**
   * Constructs a cluster list path attribute given a list of cluster numbers.
   *
   * @param l  A list of cluster numbers.
   */
ClusterList::ClusterList(intVector l):Attribute(true, false, false){
  list = l;
} 

  // ----- constructor ClusterList ----------------------------------------- //
  /**
   * Constructs a cluster list path attribute given a char stream.
   *
   * @param stream  A char stream contains the list of cluster numbers.
   */
ClusterList::ClusterList(char * stream):Attribute(true, false, false){
  // temporary do nothing
}

ClusterList::~ClusterList(){
}

  // ----- ClusterList.length ---------------------------------------------- //
  /**
   * Returns the length of the cluster list (number of cluster numbers that it
   * contains).
   *
   * @return  the length of the cluster list
   */
int ClusterList::length(){
  return list.size();
}

  // ----- ClusterList.append ---------------------------------------------- //
  /**
   * Appends a cluster number to the cluster list.
   *
   * @param cnum  The cluster number to append.
   */
void ClusterList::append(int cnum){
  list.push_back(cnum);
}    

  // ----- ClusterList.contains -------------------------------------------- //
  /**
   * Determines whether or not the cluster list contains a given cluster
   * number.
   *
   * @param cnum  The cluster number to look for in the cluster list.
   * @return true only if the cluster number is in the cluster list
   */
bool ClusterList::contains(int cnum){
   for (int i=0; i<list.size(); i++) {
      if ( list[i] == cnum) {
        return true;
      }
   }
   return false;
}

  // ----- ClusterList.copy ------------------------------------------------ //
  /**
   * Constructs and returns a copy of the attribute.
   *
   * @return a copy of the attribute
   */    
Attribute * ClusterList::copy(){
    intVector l;
    for (int i=0; i<list.size(); i++) {
      l.push_back(list[i]);
    }
    return new ClusterList(l);
}

  // ----- ClusterList.equals ---------------------------------------------- //
  /**
   * Determines whether or not this path attribute is equivalent to another.
   *
   * @param attrib  A path attribute to compare to this one.
   * @return true only if the two attributes are equivalent
   */
bool ClusterList::equals(Attribute *attrib){
    if  ( attrib == NULL  ||
          !(typeid(*attrib) == typeid(ClusterList))) {
      return false;
    }
    ClusterList * cl = (ClusterList*)attrib;
    if (list.size() != cl->list.size()) {
      return false;
    }
    for (int i=0; i<list.size(); i++) {
      if (!list[i] == cl->list[i]) {
        return false;
      }
    }
    return true;
}

// ----- ClusterList.bytecount ------------------------------------------- //
  /**
   * Returns the number of octets (bytes) needed to represent this cluster list
   * path attribute in an update message.  The number is the sum of the two
   * octets needed for the attribute type (which contains attribute flags and
   * the attribute type code), the one or two octets needed for the attribute
   * length, and the variable number of octets needed for the attribute value.
   *
   * @return the number of octets (bytes) needed to represent this cluster list
   *         path attribute in an update message
   */
int ClusterList::bytecount() {
 int octets = 2;             // 2 octets for the attribute type
    octets += 4*list.size(); // 4 octets per cluster number
    if (octets > 255) {      // 1 or 2 octets for the attribute length field
      octets += 2;
    } else {
      octets++;
    }
    return octets;
}

  // ----- ClusterList.toString -------------------------------------------- //
  /**
   * Returns the cluster list as a string.  The string is a list of integers
   * separated by spaces.  There is no space following the last integer.
   *
   * @return the cluster list as a string
   */
string  ClusterList::toString() {
  char cstr[16] = "" ;
  string str = "";
  for (int i=0; i<list.size(); i++) {
    if (i != 0) {
      str += " ";
    }
    sprintf(cstr,"%d",list[i]);
    str += cstr;
  }
  return str;
}

  // ----- ClusterList.toStream -------------------------------------------- //
  /**
   * Returns the cluster list as a char stream.  
   *
   * @return the cluster list as a char stream
   */
void ClusterList::toStream(char * stream){
  int i;
  char * c_flag= new char[1];
  unsigned int a = 255, b=255, c=255, d=255;
  a <<= 24;
  b <<= 16;
  c <<= 8;

  Attribute::toStream(c_flag);
  stream[0] = c_flag[0];
  stream[1] = ClusterList::TYPECODE;
  if (bytecount() <= 256)  {     // just one octect for length field
    stream[2] = bytecount();     
    for (i=0;i<list.size();i++) {
      stream[3+i*4]= (list[i] & a) >>24;
      stream[4+i*4]= (list[i] & b) >>16;
      stream[5+i*4]= (list[i] & c) >>8;
      stream[6+i*4]= (list[i] & d);
    }
  } else {              //two octects for length field
    stream[2] = bytecount()/256;
    stream[3] = bytecount()%256;       
    for (i=0;i<list.size();i++) {
      stream[4+i*4]= (list[i] & a) >>24;
      stream[5+i*4]= (list[i] & b) >>16;
      stream[6+i*4]= (list[i] & c) >>8;
      stream[7+i*4]= (list[i] & d);
    } 
  }
  delete [] c_flag;     
}
