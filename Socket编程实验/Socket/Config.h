#pragma once
#include <string>
#include <winsock2.h>

using namespace std;

//����������Ϣ
class Config
{
public:
	static const int MAXCONNECTION;		//���������
	static const long BUFFERLENGTH;		//��������С
    static  string SERVERADDRESS;  //��������ַ
	static  int PORT;				//�������˿�
	static  string ABSOLUTEADDRESS; //�ļ���ַ
	static const u_long BLOCKMODE;			//SOCKET����ģʽ
private:
	Config(void);
	~Config(void);
};
