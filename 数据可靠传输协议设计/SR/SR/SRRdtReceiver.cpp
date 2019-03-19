#include "stdafx.h"
#include "Global.h"
#include "SRRdtReceiver.h"
#define MAX_NUM 8
#define WIN_SIZE 4

SRRdtReceiver::SRRdtReceiver():maxnum(MAX_NUM),winsize(WIN_SIZE)
{
	AckPkt.acknum = -1; //��ʼ״̬�£��ϴη��͵�ȷ�ϰ���ȷ�����Ϊ-1��ʹ�õ���һ�����ܵ����ݰ�����ʱ��ȷ�ϱ��ĵ�ȷ�Ϻ�Ϊ-1
	AckPkt.checksum = 0;
	AckPkt.seqnum = -1;	//���Ը��ֶ�
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
	//���У����Ƿ���ȷ
	int checkSum = pUtils->calculateCheckSum(packet);

	//���У�����ȷ��ͬʱ�յ����ĵ���ŵ��ڽ��շ��ڴ��յ��ı������һ��
	if (checkSum == packet.checksum) {
		//���ڴ�����
		if (pos(rev_start, maxnum, winsize, packet.seqnum))
		{
			pUtils->printPacket("���շ���ȷ�յ����ͷ��ı���", packet);
			if (revcomf[packet.seqnum] == 0)
			{
				printf("......................\n");
				packs[(packet.seqnum - rev_start + maxnum) % maxnum] = packet;
				revcomf[packet.seqnum] = 1;
			}
			AckPkt.acknum = packet.seqnum; //ȷ����ŵ����յ��ı������
			AckPkt.checksum = pUtils->calculateCheckSum(AckPkt);
			pUtils->printPacket("���շ�����ȷ�ϱ���", AckPkt);
			pns->sendToNetworkLayer(SENDER, AckPkt);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�

			if (revcomf[rev_start] == 1)
			{
				cout << "�ƶ���������Ҫȷ�ϵ����ݣ�";
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
						printf("�����ϴ�Ӧ�ò�\n");
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
			AckPkt.acknum = packet.seqnum; //ȷ����ŵ����յ��ı������
			AckPkt.checksum = pUtils->calculateCheckSum(AckPkt);
			pUtils->printPacket("���շ�����ȷ�ϱ���", AckPkt);
			pns->sendToNetworkLayer(SENDER, AckPkt);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�
		}
	}
	else 
	{
		pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet);
	}
}