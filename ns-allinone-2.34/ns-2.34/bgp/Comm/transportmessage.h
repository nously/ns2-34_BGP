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


#ifndef TRANSPORTMESSAGE_H
#define TRANSPORTMESSAGE_H

#include "bgpmessage.h"

/** A BGP transport message.  It is used internally by BGP to indicate when
 *  transport messages are received on a socket.
 */
class TransportMessage : public BGPMessage  {
public: 
	TransportMessage(int t, string nhipre);
	virtual ~TransportMessage();
  
public:    
  /** Indicates the type of transport message.  Possible values corresponding
   *  to "open", "close", "open fail", and "fatal error" are enumerated in
   *  rtProtoBGP. */
  int trans_type;
  
  /** The NHI prefix of the neighbor/peer with whom this transport message is
   *  associated. */
  string nh;
};

#endif
