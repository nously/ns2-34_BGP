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
 * Authors: Rob Ballantyne <ballanty@cs.sfu.ca>, Tony Dongliang Feng
 * <tdfeng@cs.sfu.ca>.
 *
 */

#ifndef classifier_ipv4src_h
#define classifier_ipv4src_h

#include "classifier-ipv4.h"
#include <iostream> // Changed by Reza Sahraei
#include "tclcl.h"    

class IPv4SrcClassifier : public IPv4Classifier {
  public:
	IPv4SrcClassifier(){}
  protected:
	int classify(Packet *p);        
};

#endif
