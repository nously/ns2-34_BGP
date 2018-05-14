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

#include "community.h" 

  // ----- constructor Community ------------------------------------------- //
  /**
   * Constructs a community path attribute with the given list of community
   * values.
   *
   * @param sv  A vector of the community values.
   */
Community::Community(strVector sv) :Attribute(true, false, false){
  vals = sv;
}

Community::~Community(){
}

 // ----- Community.equals ------------------------------------------------ //
  /**
   * Determines whether or not this path attribute is equivalent to another.
   *
   * @param attrib  A path attribute to compare to this one.
   * @return true only if the two attributes are equivalent
   */
bool Community::equals(Attribute * attrib){
    if ( attrib == NULL ||
        !(typeid(*attrib) == typeid(Community)))
    return false;

    Community * com = (Community *) attrib; 
    if (vals.size() != com->vals.size()) {
      return false;
    }

    bool found;
    for (int i=0; i<vals.size(); i++) {
        found = false;
        for (int j=0; j<com->vals.size(); j++) {
          found = found || (vals[i] == com->vals[j]);
        }
        if (!found) {
          return false;
        }
    }
    return true;
}

 // ----- Community.bytecount --------------------------------------------- //
  /**
   * Returns the number of octets (bytes) needed to represent this community
   * path attribute in an update message.  The number is the sum of the two
   * octets needed for the attribute type (which contains attribute flags and
   * the attribute type code), the one or two octets needed for the attribute
   * length, and the variable number of octets needed for the attribute value.
   *
   * @return the number of octets (bytes) needed to represent this community
   *         path attribute in an update message
   */
int Community::bytecount()  {
    int octets = 2;          // 2 octets for the attribute type
    octets += 4*vals.size(); // 4 octets per community value
    if (octets > 255) {      // 1 or 2 octets for the attribute length field
      octets += 2;
    } else {
      octets++;
    }
    return octets;  
}   

  // ----- Community.copy -------------------------------------------------- //
  /**
   * Constructs and returns a copy of the attribute.
   *
   * @return a copy of the attribute
   */
Attribute * Community::copy(){
    strVector strv;
    for (int i=0; i<vals.size(); i++) {
      strv.push_back(vals[i]);
    }
    return new Community(strv);
}          

  // ----- Community.toString ---------------------------------------------- //
  /**
   * Returns this path attribute as a string.
   *
   * @return the attribute as a string
   */
string Community::toString(){
   string str = "";
    for (int i=0; i<vals.size(); i++) {
      if (i != 0) {
        str += " ";
      }
      str += vals[i];
    }
    return str;
}

