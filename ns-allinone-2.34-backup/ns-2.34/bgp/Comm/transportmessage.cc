/***************************************************************************
                          transportmessage.cpp  -  description
                             -------------------
    begin                : Mon Jun 2 2003
    copyright            : (C) 2003 by 
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "transportmessage.h"

  // ----- constructor TransportMessage ------------------------------------ //
  /**
   * Constructs a transport message given a type code and peer NHI prefix.
   *
   * @param t       The type of the transport message.
   * @param nhipre  The NHI prefix of the neighbor/peer to whom this message
   *                applies.
   */
TransportMessage::TransportMessage(int t, string nhipre)
  :BGPMessage(BGPMessage::TRANSPORT, nhipre) {
    trans_type = t;
    nh = nhipre;
}

TransportMessage::~TransportMessage(){
}

