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

#ifndef TIMEOUTMESSAGE_H
#define TIMEOUTMESSAGE_H

#include "../Comm/bgpmessage.h"

/**
 * Used to notify BGP that a timeout has occurred (one of the BGP
 * timers has expired).
 */
class TimeoutMessage : public BGPMessage  {
public: 
	TimeoutMessage(int tt, string nhipre);
	~TimeoutMessage();

public:
  /** Indicates the type of timeout that occurred (possible types are
   *  listed in class rtProtoBGP). */
  int to_type;
  
  /** The prefix of the peer to whom this timeout is relevant. */
  string nh;
};

#endif
