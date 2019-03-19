#ifndef STOP_WAIT_RDT_SENDER_H
#define STOP_WAIT_RDT_SENDER_H
#include "TCPSender.h"
#include "DataStructure.h"
#include <vector>
class TCPRdtSender :public TCPSender
{
private:
	int expectSequenceNumberSend;	// ��һ��������� 
	bool waitingState;				// �Ƿ��ڵȴ�Ack��״̬
	std::vector<Packet> Packs;//�������
	int start;
	const int maxnum;
	const int winsize;                     //Ԫ���ڴ����е�λ��
	int timer;
	int *surplusAck;
public:

	bool getWaitingState();
	bool send(Message &message);						//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(Packet &ackPkt);						//����ȷ��Ack������NetworkServiceSimulator����	
	void timeoutHandler(int seqNum);					//Timeout handler������NetworkServiceSimulator����

public:
	TCPRdtSender();
	virtual ~TCPRdtSender();
};

#endif

