#include "stdafx.h"
#include "Global.h"
#include "TCPRdtSender.h"
#define WIN_SIZE 4
#define MAX_NUM 8

TCPRdtSender::TCPRdtSender() :expectSequenceNumberSend(0), waitingState(false), winsize(WIN_SIZE), maxnum(MAX_NUM)
{
	start = 0;
	surplusAck = new int[maxnum];
}


TCPRdtSender::~TCPRdtSender()
{
	delete[]surplusAck;
}



bool TCPRdtSender::getWaitingState() {
	return ((expectSequenceNumberSend - start + maxnum) % maxnum >= winsize);
}



bool TCPRdtSender::send(Message &message) {
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

int surplus(int start, int maxnum, int winsize, int num)
{
	if (start < (start + winsize) % maxnum)
	{
		if (start < num)
			return 1;
		else
			return 0;
	}
	else
	{
		if (start < num || num <= (start + winsize) % maxnum)
			return 1;
		else
			return 0;
	}
}

void TCPRdtSender::receive(Packet &ackPkt) {
	//���У����Ƿ���ȷ
	int checkSum = pUtils->calculateCheckSum(ackPkt);

	//���У�����ȷ������ȷ�����=���ͷ��ѷ��Ͳ��ȴ�ȷ�ϵ����ݰ����
	if (checkSum == ackPkt.checksum) {
		pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
		if (surplus(start, maxnum, winsize, ackPkt.acknum))
		{
			cout << "ȷ��ǰ��δȷ�ϱ�����ţ�" << endl;
			for (auto i : Packs)
			{
				cout << i.seqnum << " ";
			}
			cout << endl;
			for (int i = 0; i < (ackPkt.acknum - start + maxnum) % maxnum; i++)
			{
				Packs.erase(Packs.begin(), Packs.begin() + 1);
			}
			start = (ackPkt.acknum) % maxnum;
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
		else
		{
			surplusAck[ackPkt.acknum]++;
			if (surplusAck[ackPkt.acknum] == 3)
			{
				for (auto i : Packs)
				{
					if (i.seqnum == ackPkt.acknum)
					{
						pUtils->printPacket("�����ش�����", ackPkt);
						pns->sendToNetworkLayer(RECEIVER, i);
					}
				}
				surplusAck[ackPkt.acknum] = 0;
			}
		}
	}
}

void TCPRdtSender::timeoutHandler(int seqNum) {
	//Ψһһ����ʱ��,���迼��seqNum
	if (start != expectSequenceNumberSend)
	{
		pns->stopTimer(SENDER, seqNum);										//���ȹرն�ʱ��
		cout << "���ܳ�ʱ" << endl;
		pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//�����������ͷ���ʱ��
		pUtils->printPacket("���ͷ����·�������δȷ�ϱ���", Packs.front());
		pns->sendToNetworkLayer(RECEIVER, Packs.front());
	}
	else
		pns->stopTimer(SENDER, seqNum);
}
