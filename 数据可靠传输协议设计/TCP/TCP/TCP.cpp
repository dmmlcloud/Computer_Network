// StopWait.cpp : 定义控制台应用程序的入口点。
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
	delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
	delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete
	
	return 0;
}

