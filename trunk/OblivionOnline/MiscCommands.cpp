
/*

Copyright(c) 2007-2008   Julian Bangert aka masterfreek64

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

Linking OblivionOnline statically or dynamically with other modules is making a combined work based
on OblivionOnline. Thus, the terms and conditions of the GNU General Public License cover 
the whole combination.

In addition, as a special exception, the copyright holders of  OblivionOnline give you permission to 
combine OblivionOnline program with free software programs or libraries that are released under
the GNU LGPL and with code included in the standard release of Oblivion Script Extender by Ian Patterson (OBSE)
under the OBSE license (or modified versions of such code, with unchanged license). You may copy and distribute such a system 
following the terms of the GNU GPL for  OblivionOnline and the licenses of the other code concerned,
provided that you include the source code of that other code when and as the GNU GPL
requires distribution of source code.

Note that people who make modified versions of  OblivionOnline are not obligated to grant this special exception
for their modified versions; it is their choice whether to do so. 
The GNU General Public License gives permission to release a modified version without this
exception; this exception also makes it possible to release a modified version which carries 
forward this exception.
*/
#include "main.h"
#include "UserInterface.h"
#include "NetSend.h"
bool Cmd_MPSendChat_Execute (COMMAND_ARGS)
{
	if (!bIsConnected)
	{
		return true;
	}
	char message[512] = "\0";
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &message)) return true;
	NetSendChat((BYTE *)message,strlen(message));
	return true;
}

bool Cmd_MPGetDebugData_Execute (COMMAND_ARGS)
{
	return true;
}


bool Cmd_MPTotalPlayers_Execute (COMMAND_ARGS)
{
	*result = (float)TotalPlayers;
	return true;
}
bool Cmd_MPGetMyID_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPGetMyID");
		return true;
	}
	*(UINT32 *)result = GetPlayerNumberFromRefID(thisObj->refID);
	return true;
}

bool Cmd_MPShowGUI_Execute(COMMAND_ARGS)
{
	if(!bUIInitialized)
	{
		InitialiseUI();
		SetConnectionMessage(ServerIP);
	}
	return true;
}
bool Cmd_GetParentCell_Execute(COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for GetParentCell");
		return true;
	}
	*(UINT32 *)result = thisObj->parentCell->refID;
	return true;
}
CommandInfo kMPSendChatCommand =
{
	"MPSendChat",
	"Chat",
	0,
	"Sends a chat message",
	0,		// requires parent obj
	1,		// has 1 param
	kParams_OneString,	// string param table
	Cmd_MPSendChat_Execute
};


CommandInfo kMPGetDebugDataCommand =
{
	"MPGetDebugData",
	"MPGDD",
	0,
	"Gets debug data",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPGetDebugData_Execute
};




CommandInfo kMPGetMyIDCommand =
{
	"MPGetMyID",
	"MPGMID",
	0,
	"Get's the player ID of the calling NPC (other player)",
	0,		// requires parent obj
	0,		// no params
	NULL,	// no param table
	Cmd_MPGetMyID_Execute
};

CommandInfo kMPShowGUICommand =
{
	"MPShowGUI",
	"MPGUI",
	0,
	"shows the GUI",
	0,		// requires parent obj
	0,		// 1 param
	0,	// one string
	Cmd_MPShowGUI_Execute
};

CommandInfo kMPTotalPlayersCommand =
{
	"MPTotalPlayers",
	"MPTPS",
	0,
	"Returns number of players connected",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPTotalPlayers_Execute
};
CommandInfo kGetParentCellCommand =
{
	"GetParentCell",
	"GPC",
	0,
	"Gets an objects parent cell",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPTotalPlayers_Execute
};
