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
#include "aggregator.h"
#include <stdlib.h> // Added by Reza Sahraei

  // ----- constructor Aggregator ------------------------------------------ //
  /**
   * Constructs an aggregator path attribute with the AS and IP address
   * of the aggregating BGP speaker.
   *
   * @param nh   The AS NHI address prefix of the aggregating BGP speaker.
   * @param ipa  The IP address of the aggregating BGP speaker.
   */
Aggregator::Aggregator(string nh, IPaddress * ipa):Attribute(true, false, false){
    asnh = nh;
    ipaddr = new IPaddress(*ipa);
}   

  // ----- Aggregator.copy ------------------------------------------------- //
  /**
   * Constructs and returns a copy of the attribute.
   *
   * @return a copy of the attribute
   */
Aggregator::Aggregator(Aggregator * a ):Attribute(true, false, false){
    asnh = a->asnh;
    ipaddr = new IPaddress(*(a->ipaddr));
}

  // ----- constructor Aggregator ------------------------------------------ //
  /**
   * Constructs an aggregator path attribute with the char stream which contains   
   * the AS and IP of the aggregating BGP speaker.
   *
   * @param stream the char stream contain the AS and IP of the aggregating
   *  BGP speaker
   */
Aggregator::Aggregator(char * stream):Attribute(true, false, false){
  int i_asnh = ((int)stream[3])* 256 + (int)stream[4];
  char * c_asnh = new char[0];
  sprintf(c_asnh,"%d",i_asnh);
  asnh = c_asnh;
  delete c_asnh;

  char * c_ip = new char[4];
  for (int i=0;i<=4;i++) {
    c_ip[i] = stream[5+i];
  }
  ipaddr = new IPaddress(c_ip);
  delete [] c_ip;  
}

Aggregator::~Aggregator(){
}

  // ----- Aggregator.equals ----------------------------------------------- //
  /**
   * Determines whether or not this path attribute is equivalent to another.
   *
   * @param attrib  A path attribute to compare to this one.
   * @return true only if the two attributes are equivalent
   */
bool Aggregator::equals(Attribute *attrib){
   return ( attrib != NULL &&
            typeid(*attrib)==typeid(Aggregator) &&
            asnh == ((Aggregator*)attrib)->asnh &&
            ipaddr->equals(((Aggregator*)attrib)->ipaddr) );
}

  // ----- Aggregator.bytecount -------------------------------------------- //
  /**
   * Returns the number of octets (bytes) needed to represent this aggregator
   * path attribute in an update message.  The number is the sum of the two
   * octets needed for the attribute type (which contains attribute flags and
   * the attribute type code), the one octet needed for the attribute length,
   * and the six octets needed for the attribute value.
   *
   * @return the number of octets (bytes) needed to represent this aggregator
   *         discriminator path attribute in an update message
   */
int Aggregator::bytecount(){
   return 9;
}         

  // ----- Aggregator.copy ------------------------------------------------- //
  /**
   * Constructs and returns a copy of the attribute.
   *
   * @return a copy of the attribute
   */
Attribute * Aggregator::copy(){
   return new Aggregator(asnh, ipaddr);

}

  // ----- Aggregator.toString --------------------------------------------- //
  /**
   * Returns this path attribute as a string.
   *
   * @return the attribute as a string
   */
string Aggregator::toString(){
   return asnh + " " + ipaddr->toString();
}

  // ----- Aggregator.toStream --------------------------------------------- //
  /**
   * Returns this path attribute as a char stream.
   *
   * @return the attribute as char stream pointed by the parameter stream
   */
void  Aggregator::toStream(char * stream) {
  char * c_flag= new char[1];
  Attribute::toStream(c_flag);
  stream[0] = c_flag[0];
  stream[1] = Aggregator::TYPECODE;
  stream[2] = 9;
  stream[3] = (int)atoi(asnh.c_str())/256;
  stream[4] = (int)atoi(asnh.c_str())%256;

  char * c_ip = new char[4];
  ipaddr->val2stream(c_ip);
  for (int i=0;i<4;i++) {
    stream[5+i] = c_ip[i];
  }  
  delete [] c_flag;
  delete [] c_ip;
}

