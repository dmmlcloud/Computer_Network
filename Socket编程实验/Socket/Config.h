#pragma once
#include <string>
#include <winsock2.h>

using namespace std;

//保存配置信息
class Config
{
public:
	static const int MAXCONNECTION;		//最大连接数
	static const long BUFFERLENGTH;		//缓冲区大小
    static  string SERVERADDRESS;  //服务器地址
	static  int PORT;				//服务器端口
	static  string ABSOLUTEADDRESS; //文件地址
	static const u_long BLOCKMODE;			//SOCKET阻塞模式
private:
	Config(void);
	~Config(void);
};
