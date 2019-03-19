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
	Packet AckPkt;				//�ϴη��͵�ȷ�ϱ���

public:
	SRRdtReceiver();
	virtual ~SRRdtReceiver();

public:
	
	void receive(Packet &packet);	//���ձ��ģ�����NetworkService����
};

#endif

