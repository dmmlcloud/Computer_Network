#include "stdafx.h"
#include "Global.h"
#include "GBNRdtSender.h"
#define WIN_SIZE 4
#define MAX_NUM 8

GBNRdtSender::GBNRdtSender() :expectSequenceNumberSend(0), waitingState(false), winsize(WIN_SIZE), maxnum(MAX_NUM)
{
	start = 0;
}


GBNRdtSender::~GBNRdtSender()
{
}



bool GBNRdtSender::getWaitingState() {
	return ((expectSequenceNumberSend - start + maxnum) % maxnum >= winsize);
}



bool GBNRdtSender::send(Message &message) {
	if (this->getWaitingState()) { //���ͷ����ڵȴ�ȷ��״̬
		return false;
	}
	Packet *np = new Packet;
	np->acknum = -1; //���Ը��ֶ�
	np->seqnum = this->expectSequenceNumberSend;
	np->checksum = 0;
	memcpy(np->payload, message.data, sizeof(message.data));
	np->checksum = pUtils->calculateCheckSum(*np);

	Packs.push_back(*np);
	pUtils->printPacket("���ͷ����ͱ���", *np);

	if (start == expectSequenceNumberSend)
	{
		timer = start;
		pns->startTimer(SENDER, Configuration::TIME_OUT, timer);			//�������ͷ���ʱ��
	}
	pns->sendToNetworkLayer(RECEIVER, *np);								//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�

	expectSequenceNumberSend = (expectSequenceNumberSend + 1) % maxnum;																				
	return true;
}

void GBNRdtSender::receive(Packet &ackPkt) {
	//���У����Ƿ���ȷ
	int checkSum = pUtils->calculateCheckSum(ackPkt);

	//���У�����ȷ������ȷ�����=���ͷ��ѷ��Ͳ��ȴ�ȷ�ϵ����ݰ����
	if (checkSum == ackPkt.checksum) {
		pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
		cout << "ȷ��ǰ��δȷ�ϱ�����ţ�" << endl;
		for (auto i : Packs)
		{
			cout << i.seqnum << " ";
		}
		cout << endl;
		for (int i = 0; i < (ackPkt.acknum - start + maxnum + 1) % maxnum; i++)
		{
			Packs.erase(Packs.begin(), Packs.begin() + 1);
		}
		start = (ackPkt.acknum + 1) % maxnum;
		cout << "ȷ�Ϻ��δȷ�ϱ�����ţ�" << endl;
		for (auto i : Packs)
		{
			cout << i.seqnum << " ";
		}
		cout << endl;
		if (start == expectSequenceNumberSend)
			pns->stopTimer(SENDER, timer);		//�رն�ʱ��
		else
		{
			pns->stopTimer(SENDER, timer);									//���ȹرն�ʱ��
			timer = start;
			pns->startTimer(SENDER, Configuration::TIME_OUT, timer);			//�����������ͷ���ʱ��
		}
	}
}

void GBNRdtSender::timeoutHandler(int seqNum) {
	//Ψһһ����ʱ��,���迼��seqNum
	pns->stopTimer(SENDER, seqNum);										//���ȹرն�ʱ��
	cout << "���ܳ�ʱ" << endl;
	if (start != expectSequenceNumberSend)
	{
		pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//�����������ͷ���ʱ��
		for (auto i : Packs)
		{
			pUtils->printPacket("���ͷ����·��ͱ���", i);
			pns->sendToNetworkLayer(RECEIVER, i);
		}
	}
}
