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

#include "receive_queue.h"
#include "stdio.h"
#include "iostream" // Changed by Reza Sahraei

ReceiveQueue::ReceiveQueue()
{
  data_received = 0;
}

ReceiveQueue::~ReceiveQueue()
{       
}

void ReceiveQueue::enqueue(TcpData* data)
{  
	int tcpDataSize = data->getSize();
	char* temp = new char[tcpDataSize];
	int bytesRead = data->getData(temp);
	for(int i=0; i<bytesRead; i++)
	{
		read_buffer.push_back(temp[i]);
	}                              
	data_received += bytesRead;    	
}

void ReceiveQueue::retrieve_data(int datalen, char* dest)
{
  if(datalen > data_received)
		return;
	for(int i=0; i<datalen; i++)
	{
		dest[i] = read_buffer.front();
		read_buffer.pop_front();
	}
	data_received -= datalen;
}

bool ReceiveQueue::is_empty()
{
	return read_buffer.empty();
}
