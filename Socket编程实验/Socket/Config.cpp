#include "Config.h"
#include <string>

using namespace std;

Config::Config(void)
{
}

Config::~Config(void)
{
}

string Config::SERVERADDRESS = "127.0.0.1";	//������IP��ַ
const int Config::MAXCONNECTION = 50;				//���������5
const long Config::BUFFERLENGTH = 6553600;				//��������С256�ֽ�
int Config::PORT = 5050;						//�������˿�5050
const u_long Config::BLOCKMODE = 1;					//SOCKETΪ������ģʽ
string Config::ABSOLUTEADDRESS = "C:\\website"; //�ļ���ַ
