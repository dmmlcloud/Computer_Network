#ifndef STOP_WAIT_RDT_RECEIVER_H
#define STOP_WAIT_RDT_RECEIVER_H
#include "TCPReceiver.h"
class TCPRdtReceiver :public TCPReceiver
{
private:
	int maxnum;
	int expectSequenceNumberRcvd;	// �ڴ��յ�����һ���������
	Packet lastAckPkt;				//�ϴη��͵�ȷ�ϱ���

public:
	TCPRdtReceiver();
	virtual ~TCPRdtReceiver();

public:
	
	void receive(Packet &packet);	//���ձ��ģ�����NetworkService����
};

#endif

