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

#ifndef send_queue_h
#define send_queue_h

#include "tcp_data.h"
#include <list>
using namespace std;

typedef struct
{
	int seq;
	int data;
}SendData;

class SendQueue
{
public:
	void enqueue(int bytes,const char* const data);
	TcpData* get_data(int t_seqno, int datalen);
	bool is_empty();
	SendQueue();
	~SendQueue();

private:
	int cur_seq;
	list<SendData> dataQueue;	
};

#endif
