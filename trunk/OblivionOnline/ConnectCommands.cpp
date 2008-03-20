/*
Copyright 2008   Julian Bangert aka masterfreek64
This file is part of OblivionOnline.

OblivionOnline is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

OblivionOnline is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "main.h"
bool Cmd_MPConnect_Execute(COMMAND_ARGS)
{
	if(!bIsConnected)
	{
		_MESSAGE("Initializing Connection... \n");
		OO_Initialize();
		SOCKADDR_IN ServerAddr;
		FILE *Realmlist = fopen("realmlist.wth","r");
		if(!Realmlist)
		{
			_ERROR("File realmlist.wth could not be found");
			Console_Print("File realmlist.wth could not be found");
			return true;
		}
		int TotalServers = 0;
		fscanf(Realmlist,"%i",&TotalServers);
		_MESSAGE("Found %i servers in realmlist.wth",TotalServers);
		char IP[MAXSERVERS][15];
		unsigned short ClientPort[MAXSERVERS];
		long rc;
		for(int i=0; i<TotalServers; i++)
		{
			fscanf(Realmlist,"%s",IP[i]);
			if(!fscanf(Realmlist,"%i",&ClientPort[i]))
				ClientPort[i] = 41805;
			_MESSAGE("Connecting to %s:%i",IP[i],ClientPort[i]);
			memset(&ServerAddr,NULL,sizeof(SOCKADDR_IN));
			ServerAddr.sin_addr.s_addr = inet_addr(IP[i]);
			ServerAddr.sin_port = htons(ClientPort[i]);
			ServerAddr.sin_family = AF_INET;
			rc = connect(ServerSocket,(SOCKADDR *)&ServerAddr,sizeof(SOCKADDR));
			if(rc == SOCKET_ERROR)
			{
				_ERROR("Couldn't connect to %s:%i %u",IP[i],ClientPort[i],WSAGetLastError());
				Console_Print("Oblivion not connected to %s",IP[i]);
			}
			else 
			{
				_MESSAGE("Successfully Connected to %s:%u",IP[i],ClientPort[i]);
				outnet.SetAddress(ServerAddr);
				bIsConnected = true;
				hRecvThread = CreateThread(NULL,NULL,RecvThread,NULL,NULL,NULL);
				//hPredictionEngine = CreateThread(NULL,NULL,PredictionEngine,NULL,NULL,NULL);
				//Now try to connect with default password
				Console_Print("Oblivion connected to %s",IP[i]);
				sprintf(ServerIP,"%s",IP[i]);
				//usrInterface.SetGlobalState(true); // we start it ...
				break;
			}
		}
		fclose(Realmlist);
	}
	return true;
}

CommandInfo kMPConnectCommand =
{
	"MPConnect",
	"MPCN",
	0,
	"Connects OO to server",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPConnect_Execute
};