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


#ifndef STARTSTOPMESSAGE_H
#define STARTSTOPMESSAGE_H

#include "bgpmessage.h"

/**
 * Message from the system or a system operator to either initiate or
 * discontinue a BGP connection with a particular (potential) neighbor/peer.
 */
class StartStopMessage : public BGPMessage  {
public: 
	StartStopMessage(int typ, string nhipre);
	virtual ~StartStopMessage();
  
public:
   /** Whether this is a start or stop message.  (All possible message types and
   *  their values are listed in class rtProtoBGP). */
  int ss_type;
  
  /** The NH prefix of the neighbor/peer whose connection to whom this message
   *  applies. */
  string nh;
};

#endif
