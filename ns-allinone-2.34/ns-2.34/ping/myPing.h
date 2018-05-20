#ifndef ns_myPing_h
#define ns_myPing_h

#include "agent.h"
#include "tclcl.h"
#include "packet.h"
#include "address.h"
#include "ip.h"

class MyPingAgent;

struct hdr_myPing {
	char ret;
	double send_time;

	// Header access methods
	static int offset_; // required by PacketHeaderManager
	inline static int& offset() { return offset_; }
	inline static hdr_myPing* access(const Packet* p) {
		return (hdr_myPing*) p->access(offset_);
	}
};

class MyPingAgentTimer : public TimerHandler {
    public:
	MyPingAgentTimer(MyPingAgent *t) : TimerHandler() 
								{ t_ = t; }
    protected:
	virtual void expire(Event *e);
	MyPingAgent *t_;
};

class MyPingAgent : public Agent {
    public:
	MyPingAgent();
	int command(int argc, const char*const* argv);
	void recv(Packet*, Handler*);
	void send_packets();
    protected:
	int num_packets_;
	double wait_interval_; 
	MyPingAgentTimer timer_;
	int count;
};

#endif
