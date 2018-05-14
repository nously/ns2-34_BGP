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

#include "origin.h"  

  // ----- constructor Origin ---------------------------------------------- //
  /**
   * Constructs an origin path attribute with the given type value.
   *
   * @param t  The origin type value.
   */
Origin::Origin(int t):Attribute (false, true, false){
  type = t;
}

  // ----- constructor Origin ---------------------------------------------- //
  /**
   * Constructs an origin path attribute with an char stream.
   *
   * @param stream  The char stream contains the origin attribute.
   */
Origin::Origin(char  * stream):Attribute (false, true, false){
  type = (int) stream[3];
}

Origin::~Origin(){
}

  // ----- Origin.equals --------------------------------------------------- //
  /**
   * Determines whether or not this path attribute is equivalent to another.
   *
   * @param attrib  A path attribute to compare to this one.
   * @return true only if the two attributes are equivalent
   */
bool Origin::equals(Attribute * attrib){
  return ( attrib != NULL &&
           typeid(*attrib) == typeid (Origin) &&
           type == ((Origin*)attrib)->type );
}

  // ----- Origin.bytecount ------------------------------------------------ //
  /**
   * Returns the number of octets (bytes) needed to represent this origin path
   * attribute in an update message.  The number is the sum of the two octets
   * needed for the attribute type (which contains attribute flags and the
   * attribute type code), the one octet needed for the attribute length, and
   * the one octet needed for the attribute value.
   *
   * @return the number of octets (bytes) needed to represent this origin path
   *         attribute in an update message
   */
int Origin::bytecount() {
    return 4;
    
}

  // ----- Origin.copy ----------------------------------------------------- //
  /**
   * Constructs and returns a copy of the attribute.
   *
   * @return a copy of the attribute
   */
Attribute * Origin::copy(){
  return new Origin(type);
}


  // ----- Origin.toString ------------------------------------------------- //
  /**
   * Returns this path attribute as a string.
   *
   * @return the attribute as a string
   */
string Origin::toString(){ 
   switch (type) {
    case IGP:  return "IGP";
    case EGP:  return "EGP";
    case INC:  return "INC";
    default:   return "";
   }
}

  // ----- Origin.toStream ------------------------------------------------- //
  /**
   * Returns this path attribute as a char stream.
   *
   * @return the attribute as a char stream which pointed by parameter stream
   */  
void Origin::toStream(char * stream) {
  char * c_flag = new  char[1];
  Attribute::toStream(c_flag);   
  stream[0] = c_flag[0];
  stream[1] = Origin::TYPECODE;
  stream[2] = 4;
  stream[3] = type;
  delete [] c_flag; 
}
