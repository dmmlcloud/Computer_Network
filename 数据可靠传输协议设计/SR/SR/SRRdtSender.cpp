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
	if (this->getWaitingState()) { //发送方处于等待确认状态
		return false;
	}
	else
	{
		Packet *np = new Packet;
		np->acknum = -1; //忽略该字段
		np->seqnum = this->expectSequenceNumberSend;
		np->checksum = 0;
		memcpy(np->payload, message.data, sizeof(message.data));
		np->checksum = pUtils->calculateCheckSum(*np);

		Packs.push_back(*np);
		pUtils->printPacket("发送方发送报文", *np);

		timer = expectSequenceNumberSend;
		pns->startTimer(SENDER, Configuration::TIME_OUT, timer);			//启动发送方定时器
		pns->sendToNetworkLayer(RECEIVER, *np);								//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方

		expectSequenceNumberSend = (expectSequenceNumberSend + 1) % maxnum;
		return true;
	}
}

void SRRdtSender::receive(Packet &ackPkt) {
	//检查校验和是否正确
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	//如果校验和正确，并且确认序号=发送方已发送并等待确认的数据包序号
	if (checkSum == ackPkt.checksum && ((ackPkt.acknum - start + maxnum) % maxnum) < winsize) {
		pUtils->printPacket("发送方正确收到确认", ackPkt);
		ackcomf[ackPkt.acknum % maxnum] = 1;
		cout << "确认前的窗口内报文序号：" << endl;
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
		cout << "确认后的窗口内报文序号：" << endl;
		for (auto i : Packs)
		{
			cout << i.seqnum << " ";
		}
		cout << endl;
		
		pns->stopTimer(SENDER, ackPkt.acknum);		//关闭定时器
	}
}

void SRRdtSender::timeoutHandler(int seqNum) {
	//唯一一个定时器,无需考虑seqNum
	pns->stopTimer(SENDER, seqNum);										//首先关闭定时器
	cout << "接受ack超时" << endl;
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//重新启动发送方定时器
	for (auto i : Packs)
	{
		if (i.seqnum == seqNum)
		{
			pUtils->printPacket("发送方重新发送报文", i);
			pns->sendToNetworkLayer(RECEIVER, i);
		}
	}
}
