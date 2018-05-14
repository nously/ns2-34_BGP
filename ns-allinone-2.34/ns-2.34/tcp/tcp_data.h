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
 * Authors: Zheng Wang, Tony Dongliang Feng <tdfeng@cs.sfu.ca>
 *
 */

#ifndef tcp_data_h
#define tcp_data_h

#include "ns-process.h"

class TcpData  : public AppData
{
private:
	int size;
	char* content;

public:
	TcpData(int size, char* data);
	~TcpData();
	int getSize();
	int getData(char* buffer);
	TcpData* copy();
};
#endif
