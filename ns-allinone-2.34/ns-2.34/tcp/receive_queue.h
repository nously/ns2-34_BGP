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

#ifndef receive_queue_h
#define receive_queue_h

#include "tcp_data.h"
#include <list>
using namespace std;

class ReceiveQueue
{
private:
	int data_received;
	list<TcpData*> dataQueue;
	list<char> read_buffer;

public:
	ReceiveQueue();
	~ReceiveQueue();

public:
	void enqueue(TcpData* data);
	void retrieve_data(int datalen,char* dest);
	int dataReceived() {return data_received;}
	bool is_empty();
};

#endif
