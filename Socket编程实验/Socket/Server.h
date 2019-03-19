#pragma once
#include <winsock2.h>
#include <list>
#include <string>
#include <map>

using namespace std;

class Server;

//��Ϣ
struct Message
{
	int id;
	SOCKET skt;
	string data;
	Server * server_pointer;
};

//������
class Server
{
private:
	bool State;
	SOCKET srvSocket;			//������socket
	char* recvBuf;				//���ܻ�����
	fd_set rfds;				//���ڼ��socket�Ƿ������ݵ����ĵ��ļ�������������socket������ģʽ�µȴ������¼�֪ͨ�������ݵ�����
	fd_set wfds;				//���ڼ��socket�Ƿ���Է��͵��ļ�������������socket������ģʽ�µȴ������¼�֪ͨ�����Է������ݣ�
	sockaddr_in srvAddr;		//��������IP��ַ
	list<SOCKET> *sessions;							//��ǰ�ĻỰsocket����
	list<SOCKET> *closedSessions;					//������ʧЧ�ĻỰsocket����
	list<string> *rcvedMessages;					//�ѽ��յ��Ŀͻ�����Ϣ����
	int numOfSocketSignaled;						//�ɶ���д������������socket����
	map<SOCKET,string> *clientAddrMaps;				//�ͻ��˵�ַ�б���ַ��(key,value)����ʽ���棬keyΪSOCKET���ͣ�valueΪstring����
	HANDLE mainThread;
protected:
	virtual void AddRecvMessage(string str);								//���յ��Ŀͻ�����Ϣ���浽��Ϣ����
	virtual void AddSession(SOCKET session);								//���µĻỰsocket�������
	virtual void AddClosedSession(SOCKET session);							//��ʧЧ�ĻỰsocket�������
	virtual void RemoveClosedSession(SOCKET closedSession);					//��ʧЧ��SOCKET�ӻỰsocket����ɾ��
	virtual void RemoveClosedSession();										//��ʧЧ��SOCKET�ӻỰsocket����ɾ��
	virtual void recvMessage(SOCKET s, Message * msg);										//��SOCKET s������Ϣ
	virtual void sendMessage(SOCKET s,string msg);							//��SOCKET s������Ϣ
	virtual string  GetClientAddress(SOCKET s);								//�õ��ͻ���IP��ַ
	virtual string  GetClientAddress(map<SOCKET,string> *maps,SOCKET s);	//�õ��ͻ���IP��ַ
	virtual void  ReceieveMessageFromClients(Message * msg);								//���ܿͻ��˷�������Ϣ
	virtual int AcceptRequestionFromClient();								//�ȴ��ͻ�����������
	
	static DWORD WINAPI Server::MainThread(LPVOID lvParamter);
	static DWORD WINAPI createRespond(LPVOID bf);


public:
	Server(void);
	virtual ~Server(void);
	virtual int WinsockStartup();		//��ʼ��Winsock
	virtual int ServerStartup();		//��ʼ��Server����������SOCKET���󶨵�IP��PORT
	virtual int ListenStartup();		//��ʼ�����ͻ�������
	virtual int Loop();					//ѭ��ִ��"�ȴ��ͻ�������"->"�������ͻ�ת����Ϣ"->"�ȴ��ͻ�����Ϣ"
	virtual void CreateMainThread();
	virtual void ExitServer();
};