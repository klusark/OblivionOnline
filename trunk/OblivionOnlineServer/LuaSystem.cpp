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
#include "LuaSystem.h"
#include "DefaultSettings.h"
LuaSystem::LuaSystem(GameServer *gs)
{
	m_GS = gs;
	m_Lua = lua_open();
	luaopen_base(m_Lua);
}

LuaSystem::~LuaSystem(void)
{
	lua_close(m_Lua);
}
bool LuaSystem::RunStartupScripts(std::string Name)
{
	DefaultSettings();
	return luaL_dofile(m_Lua,Name.c_str());
}
bool LuaSystem::RunScriptLine(std::string Line)
{
	return luaL_dostring(m_Lua,Line.c_str());
}
lua_Number LuaSystem::GetNumeric(std::string Name)
{
	lua_getglobal(m_Lua,Name.c_str());
	return lua_tonumber(m_Lua,lua_gettop(m_Lua));
}
lua_Integer LuaSystem::GetInteger(std::string Name)
{
	lua_getglobal(m_Lua,Name.c_str());
	return lua_tointeger(m_Lua,lua_gettop(m_Lua));
}
std::string LuaSystem::GetString(std::string Name)
{
	lua_getglobal(m_Lua,Name.c_str());
	return std::string(lua_tostring(m_Lua,lua_gettop(m_Lua)));
}
void LuaSystem::SetNumeric(std::string Name, lua_Number value)
{
	lua_pushnumber(m_Lua,value);
	lua_setglobal(m_Lua,Name.c_str());
}
void LuaSystem::SetInteger(std::string Name, lua_Integer value)
{
	lua_pushinteger(m_Lua,value);
	lua_setglobal(m_Lua,Name.c_str());
}
void LuaSystem::SetString(std::string Name, std::string value)
{
	lua_pushstring(m_Lua,value.c_str());
	lua_setglobal(m_Lua,Name.c_str());
}
void LuaSystem::DefaultSettings()
{
	SetInteger("ServicePort",DEFAULT_GAMEPORT);
	SetInteger("AdminPort",DEFAULT_ADMINPORT);
	SetString("Name",DEFAULT_SERVERNAME);
	SetString("RealmlistURI",DEFAULT_REALMLIST);
	SetString("Logfile",DEFAULT_LOGFILE);
}