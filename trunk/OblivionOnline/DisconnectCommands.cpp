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
#include "UserInterface.h"
bool Cmd_MPDisconnect_Execute (COMMAND_ARGS)
{
	if(bIsConnected)
	{
		OO_Deinitialize();
		bIsConnected = false;
		Console_Print("You have disconnected");
		if(bUIInitialized)
			SetConnectionMessage("You have successfully disconnected");
	}else{
		Console_Print("You are not connected");
	}
	return true;
}

CommandInfo kMPDisconnectCommand =
{
	"MPDisconnect",
	"MPDC",
	0,
	"Disconnects the player",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPDisconnect_Execute
};
