// StopWait.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Global.h"
#include "GBNSender.h"
#include "GBNReceiver.h"
#include "GBNRdtSender.h"
#include "GBNRdtReceiver.h"


int main(int argc, char** argv[])
{
	GBNSender *ps = new GBNRdtSender();
	GBNReceiver * pr = new GBNRdtReceiver();
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("D:\\net\\GBN\\input.txt");
	pns->setOutputFile("D:\\net\\GBN\\output.txt");
	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
	delete pns;										//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete
	
	return 0;
}

