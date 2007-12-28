#include "Lua_Binding.h"
/*

Copyright 2007   Julian Bangert aka masterfreek64 
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
int ServerMessage(lua_State *L)
{
	   int ArgumentCount = lua_gettop(L);    /* number of arguments */
	   std::string Message;
       for (int i = 1; i <= ArgumentCount; i++) 
	   {         
		   if(!lua_isstring(L, i)) 
		   {
			   lua_pushstring(L, "Server can only output strings!");
			   lua_error(L);
		   }
         Message += lua_tostring(L,i);		
       }
	   Message += '\n';
	   GenericLog.DoOutput(LOG_MESSAGE,(char *)Message.c_str());
       return 0;                   /* number of results */
}
int ServerError(lua_State *L)
{
	   int ArgumentCount = lua_gettop(L);    /* number of arguments */
	   std::string Message;
       for (int i = 1; i <= ArgumentCount; i++) 
	   {         
		   if(!lua_isstring(L, i)) 
		   {
			   lua_pushstring(L, "Server can only output strings!");
			   lua_error(L);
		   }
         Message += lua_tostring(L,i);		
       }
	   Message += '\n';
	   GenericLog.DoOutput(LOG_ERROR,(char *)Message.c_str());
       return 0;                   /* number of results */
}

int ServerWarning(lua_State *L)
{
	   int ArgumentCount = lua_gettop(L);    /* number of arguments */
	   std::string Message;
       for (int i = 1; i <= ArgumentCount; i++) 
	   {         
		   if(!lua_isstring(L, i)) 
		   {
			   lua_pushstring(L, "Server can only output strings!");
			   lua_error(L);
		   }
         Message += lua_tostring(L,i);		
       }
	   Message += '\n';
	   GenericLog.DoOutput(LOG_WARNING,(char *)Message.c_str());
       return 0;                   /* number of results */
}
int LuaKick(lua_State *L)
{
   int ArgumentCount = lua_gettop(L);    /* number of arguments */
   std::string Message;    
   if(lua_isnumber(L, 1)) 
   {
	   Kick(lua_tointeger(L,1));
   }
   else if(lua_isstring(L,1))
   {
	   //TODO:Implement Kicking by name
   }
   return 0;
}
bool LuaRegisterBinding(lua_State *L)
{
	lua_pushcfunction(L,ServerMessage);
	lua_setglobal(L,"ServerMessage");
	lua_pushcfunction(L,ServerError);
	lua_setglobal(L,"ServerError");
	lua_pushcfunction(L,ServerWarning);
	lua_setglobal(L,"ServerWarning");
	lua_pushcfunction(L,LuaKick);
	lua_setglobal(L,"Kick");
	return true;
}
