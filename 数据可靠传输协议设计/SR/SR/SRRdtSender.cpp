#include "stdafx.h"
#include "Global.h"
#include "SRRdtSender.h"
#define WIN_SIZE 4
#define MAX_NUM 8

SRRdtSender::SRRdtSender() :expectSequenceNumberSend(0), waitingState(false), winsize(WIN_SIZE), maxnum(MAX_NUM)
{
	start = 0;
	ackcomf = new int[maxnum];
}


SRRdtSender::~SRRdtSender()
{
	delete[]ackcomf;
}



bool SRRdtSender::getWaitingState() {
	return ((expectSequenceNumberSend - start + maxnum) % maxnum >= winsize);
}



bool SRRdtSender::send(Message &message) {
	if (this->getWaitingState()) { //���ͷ����ڵȴ�ȷ��״̬
		return false;
	}
	else
	{
		Packet *np = new Packet;
		np->acknum = -1; //���Ը��ֶ�
		np->seqnum = this->expectSequenceNumberSend;
		np->checksum = 0;
		memcpy(np->payload, message.data, sizeof(message.data));
		np->checksum = pUtils->calculateCheckSum(*np);

		Packs.push_back(*np);
		pUtils->printPacket("���ͷ����ͱ���", *np);

		timer = expectSequenceNumberSend;
		pns->startTimer(SENDER, Configuration::TIME_OUT, timer);			//�������ͷ���ʱ��
		pns->sendToNetworkLayer(RECEIVER, *np);								//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�

		expectSequenceNumberSend = (expectSequenceNumberSend + 1) % maxnum;
		return true;
	}
}

void SRRdtSender::receive(Packet &ackPkt) {
	//���У����Ƿ���ȷ
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	//���У�����ȷ������ȷ�����=���ͷ��ѷ��Ͳ��ȴ�ȷ�ϵ����ݰ����
	if (checkSum == ackPkt.checksum && ((ackPkt.acknum - start + maxnum) % maxnum) < winsize) {
		pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
		ackcomf[ackPkt.acknum % maxnum] = 1;
		cout << "ȷ��ǰ�Ĵ����ڱ�����ţ�" << endl;
		for (auto i : Packs)
		{
			cout << i.seqnum << " ";
		}
		cout << endl;
		if (ackcomf[start % maxnum] == 1)
		{
			for (int i = 0; i < winsize; i++)
			{
				if (ackcomf[start % maxnum] == 1)
				{
					Packs.erase(Packs.begin(), Packs.begin() + 1);
					ackcomf[start] = 0;
					start = (start + 1) % maxnum;
				}
				else
					break;
			}
		}
		cout << "ȷ�Ϻ�Ĵ����ڱ�����ţ�" << endl;
		for (auto i : Packs)
		{
			cout << i.seqnum << " ";
		}
		cout << endl;
		
		pns->stopTimer(SENDER, ackPkt.acknum);		//�رն�ʱ��
	}
}

void SRRdtSender::timeoutHandler(int seqNum) {
	//Ψһһ����ʱ��,���迼��seqNum
	pns->stopTimer(SENDER, seqNum);										//���ȹرն�ʱ��
	cout << "����ack��ʱ" << endl;
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//�����������ͷ���ʱ��
	for (auto i : Packs)
	{
		if (i.seqnum == seqNum)
		{
			pUtils->printPacket("���ͷ����·��ͱ���", i);
			pns->sendToNetworkLayer(RECEIVER, i);
		}
	}
}
