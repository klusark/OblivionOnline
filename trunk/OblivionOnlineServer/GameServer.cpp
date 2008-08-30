/*
This file is part of OblivionOnline Server- An open source game server for the OblivionOnline mod
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
#include "ChatIOProvider.h"
#include <string>
#include <sstream>
#include "curl/curl.h"
GameServer::GameServer(void)
{
	//Initialize all subsystems
	/*
	1. Lua system - Configurations  Done - add operator[] perhaps
	2. Event - it triggers everything and IO needs it Done
	3. IO Done
	4 .ScreenIOProvider Done
	5 .LogIOProvider Done	
	6. Netsystem  Done TCP(DONE) UDP(DONE)
	7. Entity Manager
	8. ChatIOProvider Done
	9. PartyLine ---

	Perhaps some other stuff
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
	m_Netsystem = new NetworkSystem(this);
	m_Entities = new EntityManager(m_Evt,m_Netsystem);
	m_Netsystem->StartReceiveThreads();
	m_IO->RegisterIOProvider(new ChatIOProvider(this,m_IOSys));
	//In this thread we now run the server browser update
	AdvertiseGameServer();
}

GameServer::~GameServer(void)
{
	/*
	inverse order from construction
	*/
	delete m_Netsystem;
	delete m_Entities;
	delete m_IO;
	delete m_Evt;
	delete m_script;
}

void GameServer::AdvertiseGameServer()
{
	return;
	string RealmListURI = m_script->GetString("RealmlistURI");
	string ServerName  = m_script->GetString("Name");
	char  *EscapedName;
	if(ServerName.length() != 0)
	{
		GetIO() <<"Beginning to list server" << endl;
		CURL *curl = curl_easy_init();
			while(true){
				stringstream URL;
				CURLcode result;
				if(curl)
				{
					char *EscapedName = curl_easy_escape(curl,ServerName.c_str(),0);
					URL<<RealmListURI;
					URL << "?name=" << string( EscapedName) << "&port=" <<m_script->GetInteger("ServicePort") <<"&players=" <<
						m_Netsystem->GetPlayerCount()<<"&maxplayers="<<MAX_PLAYERS <<"&VersionMajor="
						<< VERSION_MAJOR << "&VersionMinor=" << VERSION_MINOR << "&HasPassword="<<0;
					EscapedName = (char *)calloc(URL.str().length(),sizeof(char));
					memcpy(EscapedName,URL.str().c_str(),URL.str().length());
					//TODO:  If I readd passworded servers - remove the false
					//TODO: Escape string
					curl_easy_setopt(curl,CURLOPT_URL,EscapedName);
					free(EscapedName);
					result = curl_easy_perform(curl);
					
				}
				else
				{
					GetIO() << Error<< "CURL error" <<endl;
				}
				Sleep(120000);
			}
			curl_easy_cleanup(curl);

			curl_free(EscapedName);

	}
	else
	{
		GetIO() << LogLevel::Warning <<"No ListURI set. This server will not be listed online" << endl;
	}	
}

void GameServer::DisplayBootupMessage()
{
	*m_IO << "\nOblivionOnline Server version "<<VERSION_STREAM << "\n (c) 2008 by Julian Bangert \n\n" <<
		"This program is free software: you can redistribute it and/or modify\n" <<
		"it under the terms of the GNU AFFERO GENERAL PUBLIC LICENSE as published by the \n" <<
		"Free Software Foundation, either version 3 of the License, or (at your option) \n" <<
		"any later version.\n" <<

		"This program is distributed in the hope that it will be useful,\n" <<
		"but WITHOUT ANY WARRANTY; without even the implied warranty of\n" <<
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n" <<
		"GNU AFFERO GENERAL PUBLIC LICENSE for more details.\n" <<
		"You should have received a copy of the GNU AFFERO GENERAL PUBLIC LICENSE\n" <<
		"along with this program.  If not, see <http://www.gnu.org/licenses/>.\n" << endl;
	*m_IO << "Please note that you have to offer the source codes to this server software on the web for the users to download" << endl<<
		"For the official releases this is taken care of at http://sf.net/projects/oo-open/ , however if you modify ANYTHING you have to make the COMPLETE sources(also client, as that client is a derived work of the server) available to the users"<<endl;
}