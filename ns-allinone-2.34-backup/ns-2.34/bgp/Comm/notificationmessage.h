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


#ifndef NOTIFICATIONMESSAGE_H
#define NOTIFICATIONMESSAGE_H

#include "bgpmessage.h"

/**
 * Contains all of the fields that one would find in a BGP Notification
 *  message.
 */
class NotificationMessage : public BGPMessage  {
public: 
  NotificationMessage(string nhipre, int ec, int esc);
  NotificationMessage(unsigned char * stream,string nhipre);
  int body_bytecount();
  void toStream(char * stream );
  string toString();
	virtual ~NotificationMessage();
  
public:
  /** Indicates the type of error which occurred. */
  int error_code;
  /** Provides more specific information about the nature of the
   *  error.  Interpretation varies depending on the type of error. */
  int error_subcode;
};

#endif
