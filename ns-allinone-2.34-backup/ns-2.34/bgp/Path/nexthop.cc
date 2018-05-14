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

#include "nexthop.h"

  // ----- constructor NextHop --------------------------------------------- //
  /**
   * Constructs a next hop path attribute with the given IP address.
   *
   * @param ipa  The IP address of the next hop.
   */
NextHop::NextHop(IPaddress * ipa):Attribute(false, true, false){
  ipaddr = new IPaddress(*ipa);
}

  // ----- constructor NextHop --------------------------------------------- //
  /**
   * Constructs a next hop path attribute with the given char stream.
   *
   * @param stream  The char stream contains the IP address of the next hop.
   */
NextHop::NextHop(char * stream):Attribute(false, true, false){
  char * c_ip = new char[4];
  for (int i=0;i<=4;i++) {
    c_ip[i] = stream[3+i];
  }
  ipaddr = new IPaddress(c_ip);
  delete [] c_ip;
}

NextHop::~NextHop(){
  delete ipaddr;
}

  // ----- NextHop.copy ---------------------------------------------------- //
  /**
   * Constructs and returns a copy of the attribute.
   *
   * @return a copy of the attribute
   */
Attribute * NextHop::copy(){
  return new NextHop(ipaddr);
}

  // ----- NextHop.equals -------------------------------------------------- //
  /**
   * Determines whether or not this path attribute is equivalent to another.
   *
   * @param attrib  A path attribute to compare to this one.
   * @return true only if the two attributes are equivalent
   */
bool NextHop::equals(Attribute * attrib){
   return ( attrib != NULL &&
            typeid(*attrib) == typeid (NextHop) &&
            ipaddr->equals(((NextHop*)attrib)->ipaddr) );
}

  // ----- NextHop.bytecount ----------------------------------------------- //
  /**
   * Returns the number of octets (bytes) needed to represent this next hop
   * path attribute in an update message.  The number is the sum of the two
   * octets needed for the attribute type (which contains attribute flags and
   * the attribute type code), the one octets needed for the attribute length,
   * and the four octets needed for the attribute value.
   *
   * @return the number of octets (bytes) needed to represent this next hop
   *         path attribute in an update message
   */
int NextHop::bytecount() {
  return 7;
}
     
  // ----- NextHop.toString ------------------------------------------------ //
  /**
   * Returns this path attribute as a string.
   *
   * @return the attribute as a string
   */
string NextHop::toString(){
  return ipaddr->toString();
}     

  // ----- NextHop.toStream ------------------------------------------------ //
  /**
   * Returns this path attribute as a char stream.
   *
   * @return the attribute as a char stream pointed by the parameter stream
   */
void  NextHop::toStream(char * stream) {
  char * c_flag= new char[1];
  Attribute::toStream(c_flag);
  stream[0] = c_flag[0];
  stream[1] = NextHop::TYPECODE;
  stream[2] = 7;
  ipaddr->val2stream(stream+3); 
  delete [] c_flag;    
}
