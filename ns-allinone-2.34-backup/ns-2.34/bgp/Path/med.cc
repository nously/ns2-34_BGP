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

#include "med.h"
#include <stdio.h>


  // ----- constructor MED ------------------------------------------------- //
  /**
   * Constructs a multiple exit discriminator path attribute with the given
   * value.
   *
   * @param v  The value of the multiple exit discriminator.
   */
MED::MED(int v):Attribute(true, false, false){
  val = v;
}

  // ----- constructor MED ------------------------------------------------- //
  /**
   * Constructs a multiple exit discriminator path attribute with a char stream 
   * contains the MED value.
   *
   * @param v  The char stream contains the value of the multiple exit discriminator.
   */
MED::MED(char * stream):Attribute(true, false, false){
  val = 0;
  for (int i=0;i<4;i++) {
      val = val*256 + (int) stream[i+3];
  }
}  

MED::~MED(){
}

  // ----- MED.copy -------------------------------------------------------- //
  /**
   * Constructs and returns a copy of the attribute.
   *
   * @return a copy of the attribute
   */
Attribute * MED::copy(){
  return new MED(val);
}

 // ----- MED.equals ------------------------------------------------------ //
  /**
   * Determines whether or not this path attribute is equivalent to another.
   *
   * @param attrib  A path attribute to compare to this one.
   * @return true only if the two attributes are equivalent
   */
bool MED::equals(Attribute * attrib){
  return ( attrib != NULL &&
           typeid(*attrib) == typeid (MED) &&
           val == ((MED*)attrib)->val );
}


  // ----- MED.bytecount --------------------------------------------------- //
  /**
   * Returns the number of octets (bytes) needed to represent this multiple
   * exit discriminator path attribute in an update message.  The number is the
   * sum of the two octets needed for the attribute type (which contains
   * attribute flags and the attribute type code), the one octet needed for the
   * attribute length, and the four octets needed for the attribute value.
   *
   * @return the number of octets (bytes) needed to represent this multiple
   *         exit discriminator path attribute in an update message
   */
int MED::bytecount() {
  return 7;
}

  // ----- MED.toString ---------------------------------------------------- //
  /**
   * Returns this path attribute as a string.
   *
   * @return the attribute as a string
   */
string MED::toString(){
  char str[32] = "" ;     // 4 bytes
  sprintf(str," %d",val);
  return str;
}

  // ----- MED.toStream ---------------------------------------------------- //
  /**
   * Returns this path attribute as a char stream.
   *
   * @return the attribute as a char stream pointed by the parameter stream.
   */
void MED::toStream(char * stream){
  char * c_flag= new char[1];
  Attribute::toStream(c_flag);
  stream[0] = c_flag[0];
  stream[1] = MED::TYPECODE;
  stream[2] = 7;
  
  unsigned int a = 255, b=255, c=255, d=255;
  a <<= 24;
  b <<= 16;
  c <<= 8;
  stream[3] = (val & a)>>24;
  stream[4] = (val & b)>>16;
  stream[5] = (val & c)>>8;
  stream[6] = (val & d);
  delete [] c_flag;
}
