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

#include "send_queue.h"
#include "stdio.h"
#include "iostream" // Changed by Reza Sahraei

SendQueue::SendQueue()
{
	cur_seq = 1;
  dataQueue.clear();
}

SendQueue::~SendQueue()
{ 
}

void SendQueue::enqueue(int bytes, const char* const data)
{   	
  for(int i=0; i<bytes; i++)
	{
  	SendData send_data;
		send_data.data = data[i];
 		send_data.seq = cur_seq;    
		dataQueue.push_back(send_data);
		cur_seq++;
	} 
}

TcpData* SendQueue::get_data(int t_seqno,int datalen)
{
	if(t_seqno+datalen > cur_seq) 
	{

		printf("Datalen is out of range\n");
		return NULL;
	}
	char* buffer = new char[datalen];
	list<SendData>::iterator theIterator;

	// Modified by Will Hrudey
	list<SendData>::iterator targetIterator= (list<SendData>::iterator) NULL;

	//Search the position where the seqno is equal to t_seqno
	for(theIterator=dataQueue.begin(); theIterator!=dataQueue.end(); theIterator++)
	{
		if((*theIterator).seq == t_seqno)
		{
			targetIterator = theIterator;
			break;
		}
	}

	// Modified by Will Hrudey
	if(targetIterator == (list<SendData>::iterator) NULL)
		return NULL;
	for(int i=0; i<datalen; i++)
	{ 

		buffer[i] = (*targetIterator).data;
		targetIterator++;
	}
  	
	return (new TcpData(datalen,buffer));
}

bool SendQueue::is_empty()
{
	return dataQueue.empty();
}
