all:OblivionOnlineServer

#define OblivionOnlineServer
OblivionOnlineServer: ConsoleServer.o IOSystem.o Lua_Binding.o OOHashTable.o PacketHandler.o lapi.o lauxlib.o lbaselib.o  lcode.o ldblib.o ldebug.o ldo.o ldump.o lfunc.o lgc.o linit.o liolib.o llex.o lmathlib.o lmem.o loadlib.o lobject.o lopcodes.o loslib.o lparser.o lstate.o lstring.o lstrlib.o ltable.o ltablib.o ltm.o lua.o luac.o lundump.o lvm.o lzio.o print.o 
	g++ -pthread -o OblivionOnlineServer ConsoleServer.o IOSystem.o Lua_Binding.o OOHashTable.o PacketHandler.o lapi.o lauxlib.o lbaselib.o lcode.o ldblib.o ldebug.o ldo.o ldump.o lfunc.o lgc.o linit.o liolib.o llex.o lmathlib.o lmem.o loadlib.o lobject.o lopcodes.o loslib.o lparser.o lstate.o lstring.o lstrlib.o ltable.o ltablib.o ltm.o lua.o luac.o lundump.o lvm.o lzio.o print.o


# define the files
# We do not add the headers here for simplicitys sake
#ConsoleServer.cpp
ConsoleServer.o: ConsoleServer.cpp ConsoleServer.h OOStructs.h \
	OOHashTable.h IOSystem.h lua.h luaconf.h lualib.h lauxlib.h OOPackets.h \
	PacketHandler.h Lua_Binding.h


#IOSystem.o 
IOSystem.o: IOSystem.cpp IOSystem.h

#Lua_Binding.o
Lua_Binding.o: Lua_Binding.cpp Lua_Binding.h ConsoleServer.h OOStructs.h \
	  OOHashTable.h IOSystem.h lua.h luaconf.h lualib.h lauxlib.h


#OOHashTable.o
OOHashTable.o: OOHashTable.cpp OOHashTable.h ConsoleServer.h OOStructs.h \
  IOSystem.h lua.h luaconf.h lualib.h lauxlib.h


#PacketHandler.o
PacketHandler.o: PacketHandler.cpp PacketHandler.h ConsoleServer.h \
  OOStructs.h OOHashTable.h IOSystem.h lua.h luaconf.h lualib.h lauxlib.h \
  OOPackets.h

clean:
	rm -rf *.o 
include Makefile.lua
