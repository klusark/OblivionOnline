/*
OblivionOnline Server- An open source game server for the OblivionOnline mod
Copyright (C)  2008   Julian Bangert

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.


You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "GameServer.h"
#include "LuaSystem.h"
#include "ScreenIOProvider.h"
#include "logIOProvider.h"
#include "NetworkSystem.h"
#include <string>
#include <sstream>
#include "curl/curl.h"
GameServer::GameServer(void)
{
	//Initialize all subsystems
	/*
	1. Lua system - Configurations 
	2. Event - it triggers everything and IO needs it
	3. IO
	4 .ScreenIOProvider
	5 .LogIOProvider
	5. EntityManager
	6. Netsystem - perhaps add Providers here
	7. NetIOProvider - "Partyline"

	what else do we need? 
	*/
	m_script = new LuaSystem(this);
	m_script->RunStartupScripts("ServerLaunch.lua");
	m_Evt = new EventSystem(this);
	m_IOSys = new IOSystem(m_Evt);
	m_IO = new IOStream(m_IOSys);
	m_IO->RegisterIOProvider(new ScreenIOProvider(m_IOSys,LogLevel::BootMessage));//TODO : Fix that
	DisplayBootupMessage();
	*m_IO<<BootMessage<<"Script , Event and Local IO running" << endl;
	*m_IO<<BootMessage<<"Opening Log file" << endl;
	m_IO->RegisterIOProvider(new LogIOProvider(m_IOSys,LogLevel::BootMessage,m_script->GetString("Logfile")));
	m_Entities = new EntityManager(m_Evt);
	m_Netsystem = new NetworkSystem(this);
	m_Netsystem->StartReceiveThreads();
	//In this thread we now run the server browser update
	AdvertiseGameServer();
}

GameServer::~GameServer(void)
{
	/*
	inverse order from construction
	*/
	delete m_Entities;
	delete m_IO;
	delete m_Evt;
	delete m_script;
}

void GameServer::AdvertiseGameServer()
{
	string RealmListURI = m_script->GetString("RealmlistURI");
	string ServerName  = m_script->GetString("RealmlistURI");
	char  *EscapedName;
	if(ServerName.length() != 0)
	{
			GetIO() <<"Beginning to list server" << endl;
			while(true){
				stringstream URL;
				CURL *curl = curl_easy_init();
				CURLcode result;
				if(curl)
				{
					char *EscapedName = curl_easy_escape(curl,ServerName.c_str(),0);
					
					URL<<RealmListURI;
					URL << "?name=" << string( EscapedName) << "&port=" <<m_script->GetInteger("ServicePort") <<"&players=" <<
						m_Netsystem->GetPlayerCount() <<"&VersionSuper=" << VERSION_SUPER <<"&VersionMajor="
						<< VERSION_MAJOR << "&VersionMinor=" << VERSION_MINOR << "&HasPassword="<<0;

					//TODO:  If I readd passworded servers - remove the false
					//TODO: Escape string
					
					curl_free(EscapedName);
					curl_easy_setopt(curl,CURLOPT_URL,URL.str().c_str());
					result = curl_easy_perform(curl);
					curl_easy_cleanup(curl);
				}
				else
				{
					GetIO() << Error<< "CURL error" <<endl;
				}
				Sleep(120000);
			}

	}
	else
	{
		GetIO() << LogLevel::Warning <<"No ListURI set. This server will not be listed online" << endl;
	}	
}