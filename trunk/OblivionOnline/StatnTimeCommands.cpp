
/*

Copyright(c) 2007-2008   Julian Bangert aka masterfreek64, Joseph Pearson aka chessmaster42

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
bool Cmd_MPGetStat_Execute (COMMAND_ARGS)
{
	int statNumber = 0;
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPGetStat");
		return true;
	}
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &statNumber)) return true;
	if (thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
			//float statAmount = GetStat(ActorBuf, statNumber);
		//if (statAmount == -1)
		//Console_Print("Tried to get stat of non-initialized actor");
		//*result = statAmount;
	}
	return true;
}
CommandInfo kMPSendFullStatCommand =
{
	"MPSendFullStat",
	"MPSFS",
	0,
	"Send all stats of calling ref",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPSendFullStat_Execute
};

CommandInfo kMPSyncTimeCommand =
{
	"MPSyncTime",
	"MPSTM",
	0,
	"Requests server time sync",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPSyncTime_Execute
};

CommandInfo kMPGetStatCommand =
{
	"MPGetStat",
	"MPGST",
	0,
	"Gets stats",
	0,		// requires parent obj
	1,		// has 1 param
	kParams_OneInt,	// int param table
	Cmd_MPGetStat_Execute
};