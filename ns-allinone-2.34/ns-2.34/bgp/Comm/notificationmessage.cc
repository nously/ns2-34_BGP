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
#include "notificationmessage.h"

  // ----- constructor NotificationMessage --------------------------------- //
  /**
   * Initializes member data.
   *
   * @param nh  The NH part of the NHI address of the sender of this message.
   * @param ec  The error code that this message will indicate.
   * @param esc  The error subcode that this message will indicate.
   */
NotificationMessage::NotificationMessage(string nhipre, int ec, int esc)
  :BGPMessage(BGPMessage::NOTIFICATION, nhipre){
    error_code    = ec;
    error_subcode = esc;
}

  // ----- constructor NotificationMessage ----------------------------------------- //
  /**
   * Construct a NotificationMessage from a char stream.
   *
   * @param stream   The char stream contains the open message.
   * @param nhipre   The NHI address prefix of the sender of this message.
   */
NotificationMessage::NotificationMessage(unsigned char * stream,string nhipre)
   :BGPMessage(BGPMessage::NOTIFICATION, nhipre){
    error_code = (unsigned int)stream[19];
    error_subcode = (unsigned int)stream[20];
}
     
NotificationMessage::~NotificationMessage(){
}

  // ----- NotificationMessage::body_bytecount ------------------------------ //
  /**
   * Returns the number of octets (bytes) in the message body.  It is the sum
   * of one octet for the error code, one octet for the error subcode, and a
   * variable number of octets for the data field.
   *
   * @return the number of octets (bytes) in the message
   */
int NotificationMessage::body_bytecount(){
  return 2;
}

  // ----- NotificationMessage::toStream ------------------------------------------------ //
  /**
   * Returns a char stream represenation of the message.
   *
   * @return a char stream representation of the message pointed by stream
   */
void NotificationMessage::toStream(char * stream ){
  BGPMessage::toStream(stream);
  stream[19]= error_code;
  stream[20]= error_subcode;  
}

  // ----- NotificationMessage::toString ------------------------------------------------ //
  /**
   * Returns a string briefly summarizing the message.
   *
   * @return a string representation of the message
   */
string NotificationMessage::toString() {
  char *msg = new char[50];
  sprintf(msg, ", error_code: %d, error_subcode: %d",error_code,error_subcode);
  string str = BGPMessage::toString() + string(msg); 
  return str;    
}

