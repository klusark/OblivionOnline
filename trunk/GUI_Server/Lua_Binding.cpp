#include "Lua_Binding.h"
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
