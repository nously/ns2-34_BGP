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

#include "originatorid.h"

  // ----- constructor OriginatorID ---------------------------------------- //
  /**
   * Constructs an originator ID path attribute with the given router ID.
   *
   * @param ipa  The IP address ID of the originating router.
   */
OriginatorID::OriginatorID(IPaddress * ipa):Attribute(true, false, false){
  id = new IPaddress(*ipa);
}

  // ----- constructor OriginatorID ---------------------------------------- //
  /**
   * Constructs an originator ID path attribute with the given char stream.
   *
   * @param stream  The char stream that contains the IP address ID
   *    of the originating router.
   */
OriginatorID::OriginatorID(char * stream)
  :Attribute(true, false, false){
  char * c_ip = new char[4];
  for (int i=0;i<=4;i++) {
    c_ip[i] = stream[3+i];
  }
  id = new IPaddress(c_ip);
  delete[] c_ip;
}

OriginatorID::~OriginatorID(){
   delete id;
}     

  // ----- OriginatorID.equals --------------------------------------------- //
  /**
   * Determines whether or not this path attribute is equivalent to another.
   *
   * @param attrib  A path attribute to compare to this one.
   * @return true only if the two attributes are equivalent
   */
bool OriginatorID::equals(Attribute *attrib){
  return ( attrib != NULL &&
           typeid(*attrib) == typeid (OriginatorID) &&
           id->equals( ((OriginatorID*)attrib)->id));
}

  // ----- OriginatorID.bytecount ------------------------------------------ //
  /**
   * Returns the number of octets (bytes) needed to represent this originator
   * ID attribute in an update message.  The number is the sum of the two
   * octets needed for the attribute type (which contains attribute flags and
   * the attribute type code), the one octet needed for the attribute length,
   * and the four octets needed for the attribute value.
   *
   * @return the number of octets (bytes) needed to represent this originator
   *         ID attribute in an update message
   */
int OriginatorID::bytecount() {
  return 7;
}

  // ----- OriginatorID.copy ----------------------------------------------- //
  /**
   * Constructs and returns a copy of the attribute.
   *
   * @return a copy of the attribute
   */
Attribute * OriginatorID::copy(){
  return new OriginatorID(id);
} 

  // ----- OriginatorID.toString ------------------------------------------- //
  /**
   * Returns this path attribute as a string.
   *
   * @return the attribute as a string
   */
string OriginatorID::toString(){
  return id->toString();
}


  // ----- OriginatorID.toStream ------------------------------------------ //
  /**
   * Returns this OriginatorID attribute as a char stream.
   *
   * @return the attribute as a char stream which pointed by parameter stream
   */ 
void OriginatorID::toStream(char * stream){
  char * c_flag= new char[1];
  Attribute::toStream(c_flag);
  stream[0] = c_flag[0];
  stream[1] = OriginatorID::TYPECODE;
  stream[2] = 7;
  id->val2stream(stream+3);
  delete[] c_flag; 
}  