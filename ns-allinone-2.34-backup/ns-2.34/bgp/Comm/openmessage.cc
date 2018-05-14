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
#include "openmessage.h"

  // ----- constructor OpenMessage ----------------------------------------- //
  /**
   * Initializes member data.
   *
   * @param bgp_id  The BGP ID of the BGPSession composing this message.
   * @param bgp_as  The NHI address prefix of the AS of the BGPSession
   *                composing this message.
   * @param nh      The NHI address prefix of the sender of this message.
   * @param ht      The proposed value for the Hold Timer.
   */
OpenMessage::OpenMessage(IPaddress * bgp_id, string bgp_as, string nh, long ht)
 :BGPMessage(BGPMessage::OPEN, nh) {
    as_nh     = bgp_as;
    hold_time = ht;
    bgpid     = bgp_id;    
}

OpenMessage::~OpenMessage() {
}


  // ----- constructor OpenMessage ----------------------------------------- //
  /**
   * Construct a OpenMessage from a char stream.
   *
   * @param open   The char stream contains the open message.
   * @param nh     The NHI address prefix of the sender of this message.
   */
OpenMessage::OpenMessage(unsigned char * open, string nh)
  :BGPMessage(BGPMessage::OPEN, nh){
    char * c_as_nh = new char[50];
    sprintf(c_as_nh,"%d",((unsigned int) open[20])*256 +
                         ((unsigned int)open[21]));
    as_nh = string(c_as_nh);
    hold_time = ((unsigned int) open[22])*256 + ((unsigned int)open[23]);
    char * c_bgpid = new char[4];
    for (int i=0;i<4;i++)
      c_bgpid[i] = open[24+i];
    bgpid = new IPaddress(c_bgpid);
    delete[] c_as_nh;
    delete[] c_bgpid;
}        

  // ----- OpenMessage::body_bytecount -------------------------------------- //
  /**
   * Returns the number of octets (bytes) in the message body only.  It is the
   * sum of one octet for the version, two octets for the AS number, two octets
   * for the hold time, four octets for the BGP identifier, one octet for the
   * optional parameters length, and a variable number of octets for optional
   * parameters.
   *
   * @return the number of octets (bytes) in the message body
   */
int OpenMessage::body_bytecount() {
     return 10;
}

  // ----- OpenMessage::toString ------------------------------------------------ //
  /**
   * Returns a string briefly summarizing the message.
   *
   * @return a string representation of the message
   */
string OpenMessage::toString() {
  char * msg= new char[50];
  sprintf(msg, ", hold_time: %d,",hold_time);
  string str = BGPMessage::toString() + ", as: " + as_nh + string(msg) +
               " bgpid: "+ bgpid->toString();
  delete[] msg;
  return str;
}

  // ----- OpenMessage::toStream ------------------------------------------------ //
  /**
   * Returns a char stream represenation of the message.
   *
   * @return a char stream representation of the message pointed by stream
   */
void OpenMessage::toStream(char * stream) {
    BGPMessage::toStream(stream);
    stream[19] = 4;                   // fill in BGP Version     
    stream[20] = atoi(as_nh.c_str()) / 256; //fill in 2-byte AS number
    stream[21] = atoi(as_nh.c_str()) % 256;     
    stream[22] = hold_time / 256;     // fill in 2-byte Type field
    stream[23] = hold_time % 256;
    bgpid->val2stream(stream+24);     // BGP Identifier
    stream[28] = 0;                   // Optional Parameter Length
}                             



