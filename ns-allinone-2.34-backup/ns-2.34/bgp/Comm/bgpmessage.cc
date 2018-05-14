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



#include "bgpmessage.h"
#include <stdlib.h>
#include <stdio.h>

/** String representations of the different message types. */
string typeNames[] = { "Open", "Update", "Notification", "KeepAlive",
                       "Timeout", "Transport", "Start/Stop", "Run" };
                       
  // ----- constructor BGPMessage --------------------------------------------- //
  /**
   * Constructs a BGPMessage with the given sender NHI prefix address and message
   * type.
   *
   * @param mtyp    The type of the BGPMessage.
   * @param nhipre  The NHI prefix of the router of the neighbor/peer with
   *                whom this message is associated.
   */
BGPMessage::BGPMessage(int mtyp, string nhipre) {
    type = mtyp;
    nh  = nhipre; 
}

BGPMessage::~BGPMessage(){
}

  // ----- BGPMessage.header_bytecount ---------------------------------------- //
  /**
   * Returns the number of octets (bytes) in the message header.
   *
   * @return the number of octets (bytes) in the message header
   */
int BGPMessage::header_bytecount() {
    return BGPMessage::OCTETS_IN_HEADER;
}

  // ----- BGPMessage.body_bytecount ------------------------------------------ //
  /**
   * Returns the number of octets (bytes) in the message body.
   *
   * @return the number of octets (bytes) in the message body
   */
int BGPMessage::body_bytecount(){
  return 0;
}

  // ----- BGPMessage.bytecount ----------------------------------------------- //
  /**
   * Returns the number of octets (bytes) in the message.
   *
   * @return the number of octets (bytes) in the message
   */
int BGPMessage::bytecount() {
    return header_bytecount() + body_bytecount();
}

  // ----- BGPMessage.toStream ------------------------------------------------ //
  /**
   * Returns a char stream represenation of the message.
   *
   * @return a char stream representation of the message pointed by stream
   */
void BGPMessage::toStream(char * stream) {
   for (int i=0;i<16;i++) {  // 16-byte Maker field
      stream[i]=1;
   }
   stream[16] = bytecount()/256;
   stream[17] = bytecount() % 256;
   stream[18] = type; 
}     

  // ----- BGPMessage.version ------------------------------------------------- //
  /**
   * Returns the developer's version string of this BGP-4 implementation.
   *
   * @return the developer's version string of this BGP-4 implementation
   */
string BGPMessage::version() {
    return "bgp::" + BGP_version;
}

  // ----- BGPMessage.body_bytecount_fromheader -------------------------------- //
  /**
   * Returns the body byte count given a char stream of an BGPMessage.
   *
   * @return the body byte count given a char stream of an BGPMessage
   */
int BGPMessage::body_bytecount_fromheader(char * header){
   return (((unsigned int)header[16]) * 256 + (unsigned int)header[17] -
           OCTETS_IN_HEADER);
}

  // ----- BGPMessage.bytecount_fromheader -------------------------------- //
  /**
   * Returns the byte count given a char stream of an BGPMessage.
   *
   * @return the byte count given a char stream of an BGPMessage
   */
int BGPMessage::bytecount_fromheader(char * header){
   return (((unsigned int)header[16]) * 256 + (unsigned int)header[17] );
}

  // ----- BGPMessage.type_fromheader -------------------------------- //
  /**
   * Returns the type given a char stream of an BGPMessage.
   *
   * @return the type given a char stream of an BGPMessage
   */
int BGPMessage::type_fromheader(char * header){
    return (unsigned int)header[18];
}

  // ----- BGPMessage.toString ------------------------------------------------ //
  /**
   * Returns a string briefly summarizing the message.
   *
   * @return a string representation of the message
   */

string BGPMessage::toString() {
    return "type: " + typeNames[type-1] + ", src: " + nh;
} 

  // ----- BGPMessage.print_binary ------------------------------------------------ //
  /**
   * Print out the message in char stream format, mainly for debug usage.
   */
void BGPMessage::print_binary(){
  char * header ;
  BGPMessage::toStream(header);
  for (int i=0;i<19;i++) {
    printf("%d*",(unsigned int)header[i]);
  }
}
