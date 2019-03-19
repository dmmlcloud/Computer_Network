#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "Server.h"
#include "WinsockEnv.h"
#include "Config.h"
#include <winsock2.h>
#include <algorithm>
#include <vector>
#pragma comment(lib, "Ws2_32.lib")

#pragma warning(disable:4996)


vector<string> split(const string& s, const char& delim) {
	stringstream is(s);
	string item;
	vector<string> elems;
	while (getline(is, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

Server::Server(void)
{
	this->recvBuf = new char[Config::BUFFERLENGTH]; //��ʼ�����ܻ�����
	memset(this->recvBuf,'\0',Config::BUFFERLENGTH);

	this->rcvedMessages = new list<string>();
	this->sessions = new list<SOCKET>();
	this->closedSessions = new list<SOCKET>();
	this->clientAddrMaps = new map<SOCKET,string>();
}

Server::~Server(void)
{
	//�ͷŽ��ܻ�����
	if(this->recvBuf != NULL){
		delete this->recvBuf;
		this->recvBuf = NULL;
	}
	

	//�ر�server socket
	if(this->srvSocket != NULL){
		closesocket(this->srvSocket);
		this->srvSocket = NULL;
	}
	
	//�ر����лỰsocket���ͷŻỰ����
	if(this->sessions != NULL) {
		for(list<SOCKET>::iterator itor = this->sessions->begin();itor!= this->sessions->end();itor++)
			closesocket(*itor); //�رջỰ
		delete this->sessions;  //�ͷŶ���
		this->sessions = NULL;
	}
	//�ͷ�ʧЧ�Ự����
	if(this->closedSessions != NULL){ 
		for(list<SOCKET>::iterator itor = this->closedSessions->begin();itor!= this->closedSessions->end();itor++)
			closesocket(*itor); //�رջỰ
		delete this->closedSessions;//�ͷŶ���
		this->closedSessions = NULL;
	}

	//�ͷŽ�����Ϣ����
	if(this->rcvedMessages != NULL){
		this->rcvedMessages->clear(); //�����Ϣ�����е���Ϣ
		delete this->rcvedMessages;	// �ͷ���Ϣ����
		this->rcvedMessages = NULL;
	}

	//�ͷſͻ��˵�ַ�б�
	if(this->clientAddrMaps != NULL){
		this->clientAddrMaps->clear();
		delete this->clientAddrMaps;
		this->clientAddrMaps = NULL;
	}

	WSACleanup(); //����winsock ���л���
}

//��ʼ��Winsock
int Server::WinsockStartup(){
	if(WinsockEnv::Startup() == -1) return -1;	//��ʼ��Winsock
	return 0;
}

void Server::ExitServer(void)
{
	State = false;
	printf("closing server.......\n");
	DWORD exitcode;
	GetExitCodeThread(mainThread, &exitcode);
	if (TerminateThread(mainThread, exitcode))
	{
		WaitForSingleObject(mainThread, INFINITE);
		CloseHandle(mainThread);
		printf("succeed to close server\n");
	}
	else
		printf("fail to close server\n");
}

//��ʼ��Server����������sockect���󶨵�IP��PORT
int Server::ServerStartup(){
	//���� TCP socket
	printf("Input IP and Port\n");
	cin >> Config::SERVERADDRESS >> Config::PORT >> Config::ABSOLUTEADDRESS;
	this->srvSocket = socket(AF_INET,SOCK_STREAM,0);
	if(this->srvSocket == INVALID_SOCKET){
		cout << "Server socket creare error !\n";
		WSACleanup();
		return -1;
	}
	cout<< "Server socket create ok!\n";

	//���÷�����IP��ַ�Ͷ˿ں�
	this->srvAddr.sin_family = AF_INET;
	this->srvAddr.sin_port =  htons(Config::PORT);
	this->srvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//���Զ��ҵ����������ʵ�IP��ַ
//	this->srvAddr.sin_addr.S_un.S_addr = inet_addr(Config::SERVERADDRESS.c_str()); //��������һ������IP��ַ�ķ���

	//�� socket to Server's IP and port
	int rtn = bind(this->srvSocket,(LPSOCKADDR)&(this->srvAddr),sizeof(this->srvAddr));
	if(rtn == SOCKET_ERROR){
		cout  << "Server socket bind error!\n";
		closesocket(this->srvSocket);
		WSACleanup();
		return -1;
	}

    cout<< "Server socket bind ok!\n";
	return 0;
}

//��ʼ����,�ȴ��ͻ�����������
int Server::ListenStartup(){
	int rtn = listen(this->srvSocket,Config::MAXCONNECTION);
	if(rtn == SOCKET_ERROR){
		cout << "Server socket listen error!\n";
		closesocket(this->srvSocket);
		WSACleanup();
		return -1;
	}

	cout<< "Server socket listen ok!\n";
	return 0;
}

//���յ��Ŀͻ�����Ϣ���浽��Ϣ����
void Server::AddRecvMessage(string str){
	if(!str.empty())
		this->rcvedMessages->insert(this->rcvedMessages->end(),str);

}

//���µĻỰSOCKET�������
void Server::AddSession(SOCKET session){
	if(session != INVALID_SOCKET){
		this->sessions->insert(this->sessions->end(),session);
	}
}

//��ʧЧ�ĻỰSOCKET�������
void Server::AddClosedSession(SOCKET session){
	if(session != INVALID_SOCKET){
		this->closedSessions->insert(this->closedSessions->end(),session);
	}
}

//��ʧЧ��SOCKET�ӻỰSOCKET����ɾ��
void Server::RemoveClosedSession(SOCKET closedSession){
	if(closedSession != INVALID_SOCKET){
		list<SOCKET>::iterator itor = find(this->sessions->begin(),this->sessions->end(),closedSession);
		if(itor != this->sessions->end()) 
			this->sessions->erase(itor);
	}
}

//��ʧЧ��SOCKET�ӻỰSOCKET����ɾ��
void Server::RemoveClosedSession(){
	for(list<SOCKET>::iterator itor = this->closedSessions->begin();itor != this->closedSessions->end();itor++){
		this->RemoveClosedSession(*itor);
	}
}

//��SOCKET s������Ϣ
void Server::recvMessage(SOCKET socket, Message * msg){
	int receivedBytes = recv(socket,this->recvBuf,Config::BUFFERLENGTH,0);
	if(receivedBytes == SOCKET_ERROR || receivedBytes == 0){//�������ݴ��󣬰Ѳ�������ĻỰsocekt����sessionsClosed����
		this->AddClosedSession(socket);
	}
	else
	{
		//cout << recvBuf;
		msg->data = recvBuf;
		msg->server_pointer = this;
		msg->skt = socket;
		DWORD threadid;
		HANDLE mThread = CreateThread(NULL, 0, createRespond, (LPVOID)msg, 0, &threadid);
		CloseHandle(mThread);
		memset(this->recvBuf, '\0', Config::BUFFERLENGTH);//������ܻ�����
	}
}

//������Ӧ����
DWORD WINAPI Server::createRespond(LPVOID m){
	cout << "Thread start" << endl;
	Message * ms = (Message *)m;
	cout << ms->data;
	char * ms_c = (char *)ms->data.c_str();
	char* rs = strtok(ms_c, "\n");
	cout << "IP: " << ms->server_pointer->GetClientAddress(ms->skt) << endl;
	cout << "Command:" << rs << endl << "-----------------------------------------------------------------------------------" << endl;
	vector<string> is = split(rs, ' ');
	string instruction = is[0];
	if (!strcmp(is[0].c_str(), "GET"))
	{
		string url = is[1];
		string fpath(Config::ABSOLUTEADDRESS + url);
		string connect;
		string port;
		
		//connect_type
		while (rs != NULL)
		{
			rs = strtok(NULL, "\n");
			connect = rs;
			if (connect.find("Connection") != string::npos)
				break;
		}
		string connect_type = connect;
		
		//content
		ifstream fin(fpath, ios::binary);
		if (fin.is_open())
		{
			string content((istreambuf_iterator<char>(fin)),
				istreambuf_iterator<char>());

			//file_type
			string file_type;
			if (url.find(".html") != string::npos)
				file_type = "text/html";
			else if (url.find(".jpg") != string::npos)
				file_type = "image/jpeg";
			else if (url.find(".png") != string::npos)
				file_type = "image/png";

			//respond
			fin.seekg(0, ios::end);
			string respond("HTTP/1.1 200 OK\r\nContent-Length: "
				+ to_string(fin.tellg()) + "\r\n"
				+ connect_type + "\r\n"
				+ "Content-Type: " + file_type + "\r\n\r\n"
				+ content);
			fin.close();
			ms->server_pointer->sendMessage(ms->skt, respond);
			cout << "Thread close" << endl;
			return 1;
		}
		else
		{
			cout << "Cannot find the file!" << endl;
			string respond("HTTP/1.1 404 Not Found\r\n");
			cout << respond << endl;
			ms->server_pointer->sendMessage(ms->skt, respond);
		}
		
	}
}

//��SOCKET s������Ϣ
void Server::sendMessage(SOCKET socket,string msg){
	int rtn = send(socket,msg.c_str(),msg.length(),0);
	if(rtn == SOCKET_ERROR){
		cout << "Send to client failed!" << endl;
	}
}

//�������߳�
void Server::CreateMainThread()
{
	State = true;
	DWORD thread_id;
	mainThread = CreateThread(NULL, 0, MainThread, (LPVOID)this, 0, &thread_id);
	//cout << "thread id: " << thread_id << endl;
}

DWORD WINAPI Server::MainThread(LPVOID p)
{
	Server *sp = (Server*)p;
	sp->Loop();
	return 1;
}

int Server::AcceptRequestionFromClient(){
	sockaddr_in clientAddr;		//�ͻ���IP��ַ
	int nAddrLen = sizeof(clientAddr);
	u_long blockMode = Config::BLOCKMODE;//��session socket��Ϊ������ģʽ�Լ����ͻ���������

	//���srvSocket�Ƿ��յ��û���������
	if(this->numOfSocketSignaled > 0){
		if(FD_ISSET(this->srvSocket,&rfds)){  //�пͻ�����������
			this->numOfSocketSignaled--;
			//�����Ựsocket
			SOCKET newSession = accept(this->srvSocket,(LPSOCKADDR)&(clientAddr),&nAddrLen);
			if (newSession == INVALID_SOCKET) {
				cout << "Server accept connection request error!\n";
				return -1;
			}
			//���µĻỰsocket��Ϊ������ģʽ��
			if(ioctlsocket(newSession,FIONBIO,&blockMode) == SOCKET_ERROR){
				cout << "ioctlsocket() for new session failed with error!\n";
				return -1;
			}

			//���µ�session����Ự����
			this->AddSession(newSession);
			this->clientAddrMaps->insert(map<SOCKET,string>::value_type(newSession,this->GetClientAddress(newSession)));//�����
		}
	}
	return 0;
}

void Server::ReceieveMessageFromClients(Message * msg){
	if(this->numOfSocketSignaled > 0){
		//�����Ự�б��е�����socket������Ƿ������ݵ���
		for(list<SOCKET>::iterator itor = this->sessions->begin();itor!=this->sessions->end();itor++){
			if(FD_ISSET(*itor,&rfds)){  //ĳ�Ựsocket�����ݵ���
				//��������
				this->recvMessage(*itor, msg);
			}
		}//end for
	}
}

//�õ��ͻ���IP��ַ
string  Server::GetClientAddress(SOCKET s){
	string clientAddress; //clientAddress�Ǹ����ַ����� clientAddress.empty() is true
	sockaddr_in clientAddr;
	int nameLen,rtn;

	nameLen = sizeof(clientAddr);
	rtn = getpeername(s,(LPSOCKADDR)&clientAddr,&nameLen);
	if(rtn != SOCKET_ERROR){
		clientAddress += inet_ntoa(clientAddr.sin_addr);
		clientAddress.append(":" + to_string(ntohs(clientAddr.sin_port)));
	}
	
	return clientAddress; 
}

//�õ��ͻ���IP��ַ
string  Server::GetClientAddress(map<SOCKET,string> *maps,SOCKET s){
	map<SOCKET,string>::iterator itor = maps->find(s);
	if( itor != maps->end())
		return (*itor).second;
	else{
		return string("");
	}

}

//���ܿͻ��˷�������������ݲ�ת��
int Server::Loop(){
	u_long blockMode = Config::BLOCKMODE;//��srvSock��Ϊ������ģʽ�Լ����ͻ���������
	int rtn;
	
	if( (rtn = ioctlsocket(this->srvSocket,FIONBIO,&blockMode) == SOCKET_ERROR)){ //FIONBIO��������ֹ�׽ӿ�s�ķ�����ģʽ��
		cout << "ioctlsocket() failed with error!\n";
		return -1;
	}
	cout<< "ioctlsocket() for server socket ok!Waiting for client connection and data\n";

	int msg_id = 1;

	while(State){ //�ȴ��ͻ�����������
		//���ȴӻỰSOCKET������ɾ�����Ѿ��رյĻỰsocket
		this->RemoveClosedSession();
		//Prepare the read and write socket sets for network I/O notification.
		FD_ZERO(&this->rfds);
		FD_ZERO(&this->wfds);
		//��srvSocket���뵽rfds���ȴ��û���������
		FD_SET(this->srvSocket,&this->rfds);
		//�ѵ�ǰ�ĻỰsocket���뵽rfds,�ȴ��û����ݵĵ���;�ӵ�wfds���ȴ�socket�ɷ�������
		for(list<SOCKET>::iterator itor = this->sessions->begin();itor!=this->sessions->end();itor++){
			FD_SET(*itor,&rfds);
			FD_SET(*itor,&wfds);
		}
		//�ȴ��û�����������û����ݵ�����Ựsocke�ɷ�������
		if((this->numOfSocketSignaled = select(0,&this->rfds,&this->wfds,NULL,NULL)) == SOCKET_ERROR){ //select���������пɶ����д��socket��������������rtn��.���һ�������趨�ȴ�ʱ�䣬��ΪNULL��Ϊ����ģʽ
			cout << "select() failed with error!\n";
			return -1;
		}
		
		
		//���ȼ���Ƿ��пͻ��������ӵ���
		if (this->AcceptRequestionFromClient() != 0) {
			return -1;
		}
		Message * msg = new Message;
		msg->id = msg_id++;
		this->ReceieveMessageFromClients(msg);
		//�����ܿͻ��˷���������
		
	}	

	return 0;
}