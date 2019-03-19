// StopWait.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Global.h"
#include "SRSender.h"
#include "SRReceiver.h"
#include "SRRdtSender.h"
#include "SRRdtReceiver.h"


int main(int argc, char** argv[])
{
	SRSender *ps = new SRRdtSender();
	SRReceiver * pr = new SRRdtReceiver();
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("D:\\net\\SR\\input.txt");
	pns->setOutputFile("D:\\net\\SR\\output.txt");
	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
	delete pns;										//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete
	
	return 0;
}

