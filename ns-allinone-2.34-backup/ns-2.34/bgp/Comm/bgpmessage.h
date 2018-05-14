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


#ifndef BGP_MESSAGE_H
#define BGP_MESSAGE_H

#include <string>
#include <stdlib.h>
#include <iostream> // Changed by Reza Sahraei

using namespace std;

/**
 * This class holds the header fields of a BGP message.  It serves as a parent
 * class for the more specific types of BGP messages which are derived from it
 * (Open, Update, Notification, and KeepAlive messages).  It has also also been
 * extended to serve as a timeout notification message and start/stop message.
 */
class BGPMessage {
public: 
	BGPMessage(int mtyp, string nhipre);
  virtual ~BGPMessage();  
  string version();
  int header_bytecount();
  virtual int bytecount();
  virtual int body_bytecount();
  static int body_bytecount_fromheader(char * header);
  static int bytecount_fromheader(char * header);
  static int type_fromheader(char * header);
  virtual string toString();
  virtual void toStream(char * stream);
  virtual void print_binary();
  
public:
  // ......................... constants ........................... //  
  /** Indicates that a BGP message is an Open message. */
  const static int OPEN         =  1;
  /** Indicates that a BGP message is an Update message. */
  const static int UPDATE       =  2;
  /** Indicates that a BGP message is a Notification message. */
  const static int NOTIFICATION =  3;
  /** Indicates that a BGP message is a KeepAlive message. */
  const static int KEEPALIVE    =  4;
  /** Indicates that a BGP message is a local timer expiration indicator. */
  const static int TIMEOUT      =  5;
  /** Indicates that a BGP message is a Transport message. */
  const static int TRANSPORT    =  6;
  /** Indicates that a BGP message is a Start or Stop directive. */
  const static int STARTSTOP    =  7;
  /** Indicates a 'start BGP process' directive to bring BGP into existence in
   *  the simulated network. */
  const static int RUN          =  8;
  /** The number of octets (bytes) in the standard header. */
  const static int OCTETS_IN_HEADER = 19;
  
  // ........................ member data .......................... // 
  /** The developer's version string of this implementation of BGP-4. */
  string BGP_version;
  
  /** The type code of the message. */
  int type;
  
  /** The NHI prefix of the router of the neighbor/peer with whom this message
   *  is associated.  For traditional message types (open, update,
   *  notification, and keepalive), it is the NHI prefix of the peer who sent
   *  the message.  For timeout, transport, and start/stop messages, it is the
   *  NHI prefix of the neighbor/peer that the action associated with the
   *  message is directed towards. */
  string nh;

};

#endif
