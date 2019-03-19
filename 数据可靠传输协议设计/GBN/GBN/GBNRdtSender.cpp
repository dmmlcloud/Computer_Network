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

void GBNRdtSender::receive(Packet &ackPkt) {
	//检查校验和是否正确
	int checkSum = pUtils->calculateCheckSum(ackPkt);

	//如果校验和正确，并且确认序号=发送方已发送并等待确认的数据包序号
	if (checkSum == ackPkt.checksum) {
		pUtils->printPacket("发送方正确收到确认", ackPkt);
		cout << "确认前的未确认报文序号：" << endl;
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
}

void GBNRdtSender::timeoutHandler(int seqNum) {
	//唯一一个定时器,无需考虑seqNum
	pns->stopTimer(SENDER, seqNum);										//首先关闭定时器
	cout << "接受超时" << endl;
	if (start != expectSequenceNumberSend)
	{
		pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//重新启动发送方定时器
		for (auto i : Packs)
		{
			pUtils->printPacket("发送方重新发送报文", i);
			pns->sendToNetworkLayer(RECEIVER, i);
		}
	}
}
