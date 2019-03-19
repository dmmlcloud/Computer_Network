#ifndef STOP_WAIT_RDT_SENDER_H
#define STOP_WAIT_RDT_SENDER_H
#include "SRSender.h"
#include "DataStructure.h"
#include <vector>
class SRRdtSender :public SRSender
{
private:
	int expectSequenceNumberSend;	// 下一个发送序号 
	bool waitingState;				// 是否处于等待Ack的状态
	std::vector<Packet> Packs;//存放数据
	int start;
	const int maxnum;
	const int winsize;                     //元素在窗口中的位置
	int *ackcomf;
	int timer;
public:

	bool getWaitingState();
	bool send(Message &message);						//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
	void receive(Packet &ackPkt);						//接受确认Ack，将被NetworkServiceSimulator调用	
	void timeoutHandler(int seqNum);					//Timeout handler，将被NetworkServiceSimulator调用

public:
	SRRdtSender();
	virtual ~SRRdtSender();
};

#endif

