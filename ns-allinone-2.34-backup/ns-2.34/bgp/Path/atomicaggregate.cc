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

#include "atomicaggregate.h"

  // ----- constructor AtomicAggregate ------------------------------------- //
  /**
   * Constructs an atomic aggregate path attribute.
   */
AtomicAggregate::AtomicAggregate():Attribute(false, false, false){
}

  // ----- constructor AtomicAggregate ------------------------------------- //
  /**
   * Constructs an atomic aggregate path attribute.
   */
AtomicAggregate::AtomicAggregate(char * stream)
  :Attribute(false, false, false){
}   

AtomicAggregate::~AtomicAggregate(){
}

 // ----- AtomicAggregate.equals ------------------------------------------ //
  /**
   * Determines whether or not this path attribute is equivalent to another.
   *
   * @param attrib  A path attribute to compare to this one.
   * @return true only if the two attributes are equivalent
   */
bool AtomicAggregate::equals(Attribute *attrib){     
    return ( attrib != NULL  &&
            typeid(*attrib) == typeid(AtomicAggregate));
}

  // ----- AtomicAggregate.bytecount --------------------------------------- //
  /**
   * Returns the number of octets (bytes) needed to represent this atomic
   * aggregate path attribute in an update message.  The number is the sum of
   * the two octets needed for the attribute type (which contains attribute
   * flags and the attribute type code), the one octet needed for the attribute
   * length, and the zero octets needed for the attribute value.
   *
   * @return the number of octets (bytes) needed to represent this atomic
   *         aggregate path attribute in an update message
   */
int AtomicAggregate::bytecount(){
  return 3;  
}

  // ----- AtomicAggregate.copy -------------------------------------------- //
  /**
   * Constructs and returns a copy of the attribute.
   *
   * @return a copy of the attribute
   */
Attribute * AtomicAggregate::copy(){
  return new AtomicAggregate();
}

  // ----- AtomicAggregate.toString ---------------------------------------- //
  /**
   * Returns this path attribute as a string.
   *
   * @return the attribute as a string
   */
string AtomicAggregate::toString(){
  return "";
}   

  // ----- AtomicAggregate.toStream --------------------------------------------- //
  /**
   * Returns this path attribute as a char stream.
   *
   * @return the attribute as char stream pointed by the parameter stream
   */
void AtomicAggregate::toStream(char * stream){
  char * c_flag= new char[1];
  Attribute::toStream(c_flag);
  stream[0] = c_flag[0];
  stream[1] = AtomicAggregate::TYPECODE;
  stream[2] = 3;     
  delete [] c_flag;
}

