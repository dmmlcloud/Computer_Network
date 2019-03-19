#ifndef STOP_WAIT_RDT_RECEIVER_H
#define STOP_WAIT_RDT_RECEIVER_H
#include "SRReceiver.h"
class SRRdtReceiver :public SRReceiver
{
private:
	const int maxnum;
	const int winsize;
	int rev_start;
	Packet *packs;
	int *revcomf;
	Packet AckPkt;				//上次发送的确认报文

public:
	SRRdtReceiver();
	virtual ~SRRdtReceiver();

public:
	
	void receive(Packet &packet);	//接收报文，将被NetworkService调用
};

#endif

