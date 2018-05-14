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

#include "keepalivemessage.h"         

  // ----- constructor KeepAliveMessage ------------------------------------ //
  /**
   * Constructs a KeepAlive message by calling the parent class constructor.
   *
   * @param nh  The NH part of the NHI address of the sender of this message.
   */
KeepAliveMessage::KeepAliveMessage(string nh)
  :BGPMessage(BGPMessage::KEEPALIVE, nh){
}

  // ----- KeepAliveMessage::body_bytecount --------------------------------- //
  /**
   * Returns the number of octets (bytes) in the message body, which is zero
   * for a KeepAlive message.
   *
   * @return the number of octets (bytes) in the message body
   */
int KeepAliveMessage::body_bytecount(){
  return 0;
}

KeepAliveMessage::~KeepAliveMessage(){
}





