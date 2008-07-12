#include "PluginManager.h"
#include "CommandTable.h"
#include "common/IDirectoryIterator.h"
#include "Commands_Console.h"
#include "ParamInfos.h"
#include "GameAPI.h"
#include "Utilities.h"

#ifdef OBLIVION
#include "Serialization.h"
#endif

PluginManager	g_pluginManager;

PluginManager::LoadedPlugin *	PluginManager::s_currentLoadingPlugin = NULL;
PluginHandle					PluginManager::s_currentPluginHandle = 0;

#ifdef OBLIVION
static OBSEConsoleInterface g_OBSEConsoleInterface =
{
	OBSEConsoleInterface::kVersion,
	RunScriptLine
};
#endif

static const OBSEInterface g_OBSEInterface =
{
	OBSE_VERSION_INTEGER,

#ifdef OBLIVION
	OBLIVION_VERSION,
	0,
	0,
#else
	0,
	CS_VERSION,
	1,
#endif
	PluginManager::RegisterCommand,
	PluginManager::SetOpcodeBase,
	PluginManager::QueryInterface,
	PluginManager::GetPluginHandle
};

PluginManager::PluginManager()
{
	//
}

PluginManager::~PluginManager()
{
	DeInit();
}

bool PluginManager::Init(void)
{
	bool	result = false;

	if(FindPluginDirectory())
	{
		_MESSAGE("plugin directory = %s", m_pluginDirectory.c_str());

		InstallPlugins();

		result = true;
	}

	return result;
}

void PluginManager::DeInit(void)
{
	for(LoadedPluginList::iterator iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
	{
		LoadedPlugin	* plugin = &(*iter);

		if(plugin->handle)
		{
			FreeLibrary(plugin->handle);
		}
	}

	m_plugins.clear();
}

UInt32 PluginManager::GetNumPlugins(void)
{
	UInt32	numPlugins = m_plugins.size();

	// is one currently loading?
	if(s_currentLoadingPlugin) numPlugins++;

	return numPlugins;
}

UInt32 PluginManager::GetBaseOpcode(UInt32 idx)
{
	return m_plugins[idx].baseOpcode;
}

PluginHandle PluginManager::LookupHandleFromBaseOpcode(UInt32 baseOpcode)
{
	UInt32	idx = 1;

	for(LoadedPluginList::iterator iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
	{
		LoadedPlugin	* plugin = &(*iter);

		if(plugin->baseOpcode == baseOpcode)
			return idx;

		idx++;
	}

	return kPluginHandle_Invalid;
}

PluginInfo * PluginManager::GetInfoByName(const char * name)
{
	for(LoadedPluginList::iterator iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
	{
		LoadedPlugin	* plugin = &(*iter);

		if(plugin->info.name && !strcmp(name, plugin->info.name))
			return &plugin->info;
	}

	return NULL;
}

bool PluginManager::RegisterCommand(CommandInfo * _info)
{
	ASSERT(_info);
	ASSERT_STR(s_currentLoadingPlugin, "PluginManager::RegisterCommand: called outside of plugin load");

	CommandInfo	info = *_info;

#ifndef OBLIVION
	// modify callbacks for editor

	info.unk1 = Cmd_Default_Execute;
	info.unk3 = NULL;	// not supporting this yet
#endif

	if(!info.unk2) info.unk2 = Cmd_Default_Parse;
	if(!info.shortName) info.shortName = "";
	if(!info.helpText) info.helpText = "";

	_MESSAGE("RegisterCommand %s (%04X)", info.longName, g_scriptCommands.GetCurID());

	g_scriptCommands.Add(&info);

	return true;
}

void PluginManager::SetOpcodeBase(UInt32 opcode)
{
	_MESSAGE("SetOpcodeBase %08X", opcode);

	ASSERT(opcode < 0x8000);	// arbitrary maximum for samity check
	ASSERT(opcode >= 0x2000);	// beginning of plugin opcode space
	ASSERT_STR(s_currentLoadingPlugin, "PluginManager::SetOpcodeBase: called outside of plugin load");

	if(opcode == 0x2000)
	{
		const char	* pluginName = "<unknown name>";

		if(s_currentLoadingPlugin && s_currentLoadingPlugin->info.name)
			pluginName = s_currentLoadingPlugin->info.name;

		_ERROR("You have a plugin installed that is using the default opcode base. (%s)", pluginName);
		_ERROR("This is acceptable for temporary development, but not for plugins released to the public.");
		_ERROR("As multiple plugins using the same opcode base create compatibility issues, plugins triggering this message may not load in future versions of OBSE.");
		_ERROR("Please contact the authors of the plugin and have them request and begin using an opcode range assigned by the OBSE team.");

#ifdef _DEBUG
		_ERROR("WARNING: serialization is being allowed for this plugin as this is a debug build of OBSE. It will not work in release builds.");
#endif
	}
#ifndef _DEBUG
	else	// disallow plugins using default opcode base from using it as a unique id
#endif
	{
		// record the first opcode registered for this plugin
		if(!s_currentLoadingPlugin->baseOpcode)
			s_currentLoadingPlugin->baseOpcode = opcode;
	}

	g_scriptCommands.PadTo(opcode);
	g_scriptCommands.SetCurID(opcode);
}

void * PluginManager::QueryInterface(UInt32 id)
{
	void	* result = NULL;

#ifdef OBLIVION
	switch(id)
	{
		case kInterface_Console:
			result = (void *)&g_OBSEConsoleInterface;
			break;

		case kInterface_Serialization:
			result = (void *)&g_OBSESerializationInterface;
			break;

		default:
			_WARNING("unknown QueryInterface %08X", id);
			break;
	}
#else
	_WARNING("unknown QueryInterface %08X", id);
#endif
	
	return result;
}

PluginHandle PluginManager::GetPluginHandle(void)
{
	ASSERT_STR(s_currentPluginHandle, "A plugin has called OBSEInterface::GetPluginHandle outside of its Query/Load handlers");

	return s_currentPluginHandle;
}

bool PluginManager::FindPluginDirectory(void)
{
	bool	result = false;

	// find the path <oblivion directory>/data/obse/
	std::string	oblivionDirectory = GetOblivionDirectory();
	
	if(!oblivionDirectory.empty())
	{
		m_pluginDirectory = oblivionDirectory + "Data\\OBSE\\Plugins\\";
		result = true;
	}

	return result;
}

void PluginManager::InstallPlugins(void)
{
	// avoid realloc
	m_plugins.reserve(5);

	for(IDirectoryIterator iter(m_pluginDirectory.c_str(), "*.dll"); !iter.Done(); iter.Next())
	{
		std::string	pluginPath = iter.GetFullPath();

		_MESSAGE("checking plugin %s", pluginPath.c_str());

		LoadedPlugin	plugin;
		memset(&plugin, 0, sizeof(plugin));

		s_currentLoadingPlugin = &plugin;
		s_currentPluginHandle = m_plugins.size() + 1;	// +1 because 0 is reserved for internal use

		plugin.handle = (HMODULE)LoadLibrary(pluginPath.c_str());
		if(plugin.handle)
		{
			bool		success = false;

			plugin.query = (_OBSEPlugin_Query)GetProcAddress(plugin.handle, "OBSEPlugin_Query");
			plugin.load = (_OBSEPlugin_Load)GetProcAddress(plugin.handle, "OBSEPlugin_Load");

			if(plugin.query && plugin.load)
			{
				const char	* loadStatus = NULL;

				if(plugin.query(&g_OBSEInterface, &plugin.info))
				{
					if(plugin.load(&g_OBSEInterface))
					{
						loadStatus = "loaded correctly";
						success = true;
					}
					else
					{
						loadStatus = "reported as incompatible during load";
					}
				}
				else
				{
					loadStatus = "reported as incompatible during query";
				}

				ASSERT(loadStatus);

				_MESSAGE("plugin %s (%08X %s %08X) %s",
						pluginPath.c_str(),
						plugin.info.infoVersion,
						plugin.info.name ? plugin.info.name : "<NULL>",
						plugin.info.version,
						loadStatus);
			}
			else
			{
				_MESSAGE("plugin %s does not appear to be an OBSE plugin", pluginPath.c_str());
			}
			
			if(success)
			{
				// succeeded, add it to the list
				m_plugins.push_back(plugin);
			}
			else
			{
				// failed, unload the library
				FreeLibrary(plugin.handle);
			}
		}
		else
		{
			_ERROR("couldn't load plugin %s", pluginPath.c_str());
		}
	}

	s_currentLoadingPlugin = NULL;
	s_currentPluginHandle = 0;
}

#ifdef OBLIVION

bool Cmd_IsPluginInstalled_Execute(COMMAND_ARGS)
{
	char	pluginName[256];

	*result = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &pluginName)) return true;

	*result = (g_pluginManager.GetInfoByName(pluginName) != NULL) ? 1 : 0;

	return true;
}

bool Cmd_GetPluginVersion_Execute(COMMAND_ARGS)
{
	char	pluginName[256];

	*result = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &pluginName)) return true;

	PluginInfo	* info = g_pluginManager.GetInfoByName(pluginName);
	
	*result = info ? info->version : -1;

	return true;
}

#endif

CommandInfo kCommandInfo_IsPluginInstalled =
{
	"IsPluginInstalled",
	"",
	0,
	"returns 1 if the specified plugin is installed, else 0",
	0,
	1,
	kParams_OneString,

	HANDLER(Cmd_IsPluginInstalled_Execute),
	Cmd_Default_Parse,
	NULL,
	NULL
};

CommandInfo kCommandInfo_GetPluginVersion =
{
	"GetPluginVersion",
	"",
	0,
	"returns the version of the specified plugin, or -1 if the plugin is not installed",
	0,
	1,
	kParams_OneString,

	HANDLER(Cmd_GetPluginVersion_Execute),
	Cmd_Default_Parse,
	NULL,
	NULL
};
