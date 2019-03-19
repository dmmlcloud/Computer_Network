// StopWait.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Global.h"
#include "TCPSender.h"
#include "TCPReceiver.h"
#include "TCPRdtSender.h"
#include "TCPRdtReceiver.h"


int main(int argc, char** argv[])
{
	TCPSender *ps = new TCPRdtSender();
	TCPReceiver * pr = new TCPRdtReceiver();
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("D:\\net\\TCP\\input.txt");
	pns->setOutputFile("D:\\net\\TCP\\output.txt");
	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
	delete pns;										//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete
	
	return 0;
}

