#include "Server.h"

void main(){
	Server srv;
	if(srv.WinsockStartup() == -1) return;
	if(srv.ServerStartup() == -1) return;
	if(srv.ListenStartup() == -1) return;
	srv.CreateMainThread();
	while (1)
	{
		if (GetKeyState('Q') & 0x8000)
		{
			Sleep(100);
			srv.ExitServer();
		}
		else if (GetKeyState('S') & 0x8000)
		{
			srv.CreateMainThread();
			Sleep(100);
		}
			
	}
}