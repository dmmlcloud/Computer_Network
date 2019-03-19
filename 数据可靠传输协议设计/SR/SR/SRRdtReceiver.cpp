#include "stdafx.h"
#include "Global.h"
#include "SRRdtReceiver.h"
#define MAX_NUM 8
#define WIN_SIZE 4

SRRdtReceiver::SRRdtReceiver():maxnum(MAX_NUM),winsize(WIN_SIZE)
{
	AckPkt.acknum = -1; //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
	AckPkt.checksum = 0;
	AckPkt.seqnum = -1;	//忽略该字段
	rev_start = 0;
	for(int i = 0; i < Configuration::PAYLOAD_SIZE;i++){
		AckPkt.payload[i] = '.';
	}
	AckPkt.checksum = pUtils->calculateCheckSum(AckPkt);
	revcomf = new int[maxnum];
	for (int i = 0; i < maxnum; i++)
		revcomf[i] = 0;
	packs = new Packet[winsize];
	for (int i = 0; i < winsize; i++)
	{
		packs[i] = AckPkt;
	}
}


SRRdtReceiver::~SRRdtReceiver()
{
	delete[]revcomf;
	delete[]packs;
}

int pos(int start, int maxnum, int winsize, int pac) {
	if (start < (start + winsize) % maxnum)
	{
		if (start <= pac && pac < start + winsize)
			return 1;
		else
			return 0;
	}
	else
	{
		if (pac >= start || pac < (start + winsize) % maxnum)
			return 1;
		else
			return 0;
	}
}

void SRRdtReceiver::receive(Packet &packet) {
	//检查校验和是否正确
	int checkSum = pUtils->calculateCheckSum(packet);

	//如果校验和正确，同时收到报文的序号等于接收方期待收到的报文序号一致
	if (checkSum == packet.checksum) {
		//落在窗口内
		if (pos(rev_start, maxnum, winsize, packet.seqnum))
		{
			pUtils->printPacket("接收方正确收到发送方的报文", packet);
			if (revcomf[packet.seqnum] == 0)
			{
				printf("......................\n");
				packs[(packet.seqnum - rev_start + maxnum) % maxnum] = packet;
				revcomf[packet.seqnum] = 1;
			}
			AckPkt.acknum = packet.seqnum; //确认序号等于收到的报文序号
			AckPkt.checksum = pUtils->calculateCheckSum(AckPkt);
			pUtils->printPacket("接收方发送确认报文", AckPkt);
			pns->sendToNetworkLayer(SENDER, AckPkt);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方

			if (revcomf[rev_start] == 1)
			{
				cout << "移动窗口中需要确认的内容：";
				for (int i = 0; i < winsize; i++)
				{
					if (revcomf[packs[i].seqnum] == 1)
					{
						cout << packs[i].seqnum << " ";
					}
					else
						break;
				}
				cout << endl;

				int frontnum = 0;

				for (int i = 0; i < winsize; i++)
				{
					if (revcomf[packs[i].seqnum] == 1)
					{
						printf("报文上传应用层\n");
						Message msg;
						memcpy(msg.data, packs[i].payload, sizeof(packs[i].payload));
						pns->delivertoAppLayer(RECEIVER, msg);
						revcomf[packs[i].seqnum] = 0;
						rev_start = (rev_start + 1) % maxnum;
						frontnum++;
					}
					else
						break;
				}

				for (int i = frontnum; i < winsize; i++)
				{
					packs[i - frontnum] = packs[i];
				}

			}
		}
		else
		{
			AckPkt.acknum = packet.seqnum; //确认序号等于收到的报文序号
			AckPkt.checksum = pUtils->calculateCheckSum(AckPkt);
			pUtils->printPacket("接收方发送确认报文", AckPkt);
			pns->sendToNetworkLayer(SENDER, AckPkt);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
		}
	}
	else 
	{
		pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
	}
}