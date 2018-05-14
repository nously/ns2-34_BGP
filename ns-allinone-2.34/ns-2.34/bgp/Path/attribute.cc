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


#include "attribute.h"
#include <iostream> // Changed by Reza Sahraei
#include <stdlib.h>         

  // ----- constructor Attribute ------------------------------------------- //
  /**
   * Constructs a empty path attribute, mainly for debug usage.
   *
   */
Attribute::Attribute(){
}

  // ----- constructor Attribute ------------------------------------------- //
  /**
   * Constructs a path attribute of the given type and categories.
   *
   * @param o    Whether or not this is an optional path attribute.
   * @param t    Whether or not this is a transitive path attribute.
   * @param p    Whether or not this is a partial path attribute.
   */
Attribute::Attribute(bool o,bool t,bool p) {
   opt = o;
   trans = t;
   partial = p;
}


  // ----- Attribute.bytecount --------------------------------------------- //
  /**
   * Calculates and returns the number of octets (bytes) needed to represent
   * this path attribute in an update message.  The number is the sum of the
   * two octets needed for the attribute type (which contains attribute flags
   * and the attribute type code), the one or two octets needed for the
   * attribute length, and the variable number of octets needed for the
   * attribute value.
   *
   * @return the number of octets (bytes) needed to represent this path
   *         attribute in an update message
   */
int Attribute::bytecount() {
  return 0;
}

  // ----- Attribute.toString --------------------------------------------- //
  /**
   *  @return the string representation this path attribute
   */
string Attribute::toString() {
  return "";
}

  // ----- Attribute.toStream --------------------------------------------- //
  /**
   *  @return the char stream representation this path attribute
   *          in the parameter stream
   */
void Attribute::toStream(char * stream) {
  unsigned int temp = 0;
  if (opt)
    // set the first high-order bit if it's optional 
    temp |= 128; 
  if (trans)
    // set the second high-order bit if it's transive
    temp |= 64;  
  if (partial)
    // set the third high-order bit if it's partial
    temp |= 32;  
  stream[0] = temp;
}

Attribute::~Attribute(){
}


