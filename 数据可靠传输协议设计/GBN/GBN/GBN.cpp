// StopWait.cpp : 定义控制台应用程序的入口点。
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
	delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
	delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete
	
	return 0;
}

