#ifndef STOP_WAIT_RDT_RECEIVER_H
#define STOP_WAIT_RDT_RECEIVER_H
#include "GBNReceiver.h"
class GBNRdtReceiver :public GBNReceiver
{
private:
	int maxnum;
	int expectSequenceNumberRcvd;	// �ڴ��յ�����һ���������
	Packet lastAckPkt;				//�ϴη��͵�ȷ�ϱ���

public:
	GBNRdtReceiver();
	virtual ~GBNRdtReceiver();

public:
	
	void receive(Packet &packet);	//���ձ��ģ�����NetworkService����
};

#endif

