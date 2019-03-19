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
	if (this->getWaitingState()) { //发送方处于等待确认状态
		return false;
	}
	Packet *np = new Packet;
	np->acknum = -1; //忽略该字段
	np->seqnum = this->expectSequenceNumberSend;
	np->checksum = 0;
	memcpy(np->payload, message.data, sizeof(message.data));
	np->checksum = pUtils->calculateCheckSum(*np);

	Packs.push_back(*np);
	pUtils->printPacket("发送方发送报文", *np);

	if (start == expectSequenceNumberSend)
	{
		timer = start;
		pns->startTimer(SENDER, Configuration::TIME_OUT, timer);			//启动发送方定时器
	}
	pns->sendToNetworkLayer(RECEIVER, *np);								//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方

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
	//检查校验和是否正确
	int checkSum = pUtils->calculateCheckSum(ackPkt);

	//如果校验和正确，并且确认序号=发送方已发送并等待确认的数据包序号
	if (checkSum == ackPkt.checksum) {
		pUtils->printPacket("发送方正确收到确认", ackPkt);
		if (surplus(start, maxnum, winsize, ackPkt.acknum))
		{
			cout << "确认前的未确认报文序号：" << endl;
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
			cout << "确认后的未确认报文序号：" << endl;
			for (auto i : Packs)
			{
				cout << i.seqnum << " ";
			}
			cout << endl;
			if (start == expectSequenceNumberSend)
				pns->stopTimer(SENDER, timer);		//关闭定时器
			else
			{
				pns->stopTimer(SENDER, timer);									//首先关闭定时器
				timer = start;
				pns->startTimer(SENDER, Configuration::TIME_OUT, timer);			//重新启动发送方定时器
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
						pUtils->printPacket("快速重传数据", ackPkt);
						pns->sendToNetworkLayer(RECEIVER, i);
					}
				}
				surplusAck[ackPkt.acknum] = 0;
			}
		}
	}
}

void TCPRdtSender::timeoutHandler(int seqNum) {
	//唯一一个定时器,无需考虑seqNum
	if (start != expectSequenceNumberSend)
	{
		pns->stopTimer(SENDER, seqNum);										//首先关闭定时器
		cout << "接受超时" << endl;
		pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//重新启动发送方定时器
		pUtils->printPacket("发送方重新发送最早未确认报文", Packs.front());
		pns->sendToNetworkLayer(RECEIVER, Packs.front());
	}
	else
		pns->stopTimer(SENDER, seqNum);
}
