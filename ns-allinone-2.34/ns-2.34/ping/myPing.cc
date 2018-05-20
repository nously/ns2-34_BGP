#include "myPing.h"

int hdr_myPing::offset_;

static class kangAbHeaderClass : public PacketHeaderClass {
public:
	kangAbHeaderClass() : 	PacketHeaderClass("PacketHeader/MyPing", 
					      		sizeof(hdr_myPing)) {
		bind_offset(&hdr_myPing::offset_);
	}
} class_kangAbhdrSnd;

static class MyPingAgentClass : public TclClass {
public:
	MyPingAgentClass() : TclClass("Agent/MyPing") {}
	TclObject* create(int, const char*const*) {
		return (new MyPingAgent());
	}
} class_myPing;

/* constructor*/
MyPingAgent::MyPingAgent() : Agent(PT_MYPING), timer_(this), count(0)
{  	
	bind("packetSize_",&size_);
	bind("num_packets_",&num_packets_);
	bind("wait_interval_",&wait_interval_);
}

/* if timer expires just do a timeout */
void MyPingAgentTimer::expire(Event*)
{
        t_->send_packets();
}

int MyPingAgent::command(int argc, const char*const* argv)
{
	if (argc == 2) {
	  	if (strcmp(argv[1], "send") == 0) {
	    	send_packets();
			// return TCL_OK, so the calling function knows that			// the command has been processed
	    	return (TCL_OK);
	  	}
	}
	return (Agent::command(argc, argv));
}

void MyPingAgent::send_packets()
{
	if(count++ < num_packets_)
	{
		Packet *pkt = allocpkt();
		hdr_myPing *hdr = hdr_myPing::access(pkt);
		// Set the 'ret' field to 0, so the receiving node knows
		// that it has to generate an echo packet
		hdr->ret = 0;
		// Store the current time in the 'send_time' field
		hdr->send_time = Scheduler::instance().clock();
		// Send the packet
		send(pkt, 0);
	}
	if(count < num_packets_)
		timer_.resched(wait_interval_);
}

void MyPingAgent::recv(Packet* pkt, Handler*)
{
	hdr_myPing *hdr=hdr_myPing::access(pkt);
	if (hdr->ret == 0) {
		// Send an 'echo'. First save the old packet's send_time
		double stime = hdr->send_time;
		// Discard the packet
		Packet::free(pkt);
		// Create a new packet
		Packet* pktret = allocpkt();    
		// Access the Ping header for the new packet:
		hdr_myPing* hdrret = hdr_myPing::access(pktret);
		// Set the 'ret' field to 1
		hdrret->ret = 1;                
		// Set the send_time field to the correct value
		hdrret->send_time = stime;      
		// Send the packet              
		send(pktret, 0);
	}
	else {
		// Get the receiving time of the packet
		double rcv_time = Scheduler::instance().clock();
		printf("RTT = %lf\n",rcv_time-hdr->send_time);
		// Discard the packet           
		Packet::free(pkt);
	}
}
