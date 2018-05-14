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

#include "tcp_data.h"
#include "string.h"
#include "stdlib.h"
TcpData::TcpData(int size, char *data) :AppData(TCPAPP_STRING)
{
	this->size = size;
	content = data;        
}

TcpData* TcpData::copy()
{
	char* buffer = new char[size];
	memcpy(buffer,content,size);
	return (new TcpData(size,buffer));
}

int TcpData::getData(char* buffer)
{
	for(int i=0; i<size; i++)
	{
		buffer[i] = content[i];
	}
	return size;
}

TcpData::~TcpData()
{
  if(content)
    free(content);
}

int TcpData::getSize()
{
  return size;
}
