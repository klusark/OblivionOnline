#include "PluginManager.h"
#include "CommandTable.h"
#include "common/IDirectoryIterator.h"
#include "Commands_Console.h"
#include "ParamInfos.h"
#include "GameAPI.h"

PluginManager	g_pluginManager;

static PluginInfo		* s_currentLoadingPluginInfo = NULL;

static bool RegisterCommand(CommandInfo * _info)
{
	ASSERT(_info);

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

static void SetOpcodeBase(UInt32 opcode)
{
	_MESSAGE("SetOpcodeBase %08X", opcode);

	ASSERT(opcode < 0x8000);	// arbitrary maximum for samity check
	ASSERT(opcode >= 0x2000);	// beginning of plugin opcode space

	if(opcode == 0x2000)
	{
		const char	* pluginName = "<unknown name>";

		if(s_currentLoadingPluginInfo && s_currentLoadingPluginInfo->name)
			pluginName = s_currentLoadingPluginInfo->name;

		_ERROR("You have a plugin installed that is using the default opcode base. (%s)", pluginName);
		_ERROR("This is acceptable for temporary development, but not for plugins released to the public.");
		_ERROR("As multiple plugins using the same opcode base create compatibility issues, plugins triggering this message may not load in future versions of OBSE.");
		_ERROR("Please contact the authors of the plugin and have them request and begin using an opcode range assigned by the OBSE team.");
	}

	g_scriptCommands.PadTo(opcode);
	g_scriptCommands.SetCurID(opcode);
}

#ifdef OBLIVION
static OBSEConsoleInterface g_OBSEConsoleInterface =
{
	OBSEConsoleInterface::kVersion,
	RunScriptLine
};
#endif

static void * QueryInterface(UInt32 id)
{
	void	* result = NULL;

#ifdef OBLIVION
	switch(id)
	{
		case kInterface_Console:
			result = (void *)&g_OBSEConsoleInterface;
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
	RegisterCommand,
	SetOpcodeBase,
	QueryInterface
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

bool PluginManager::FindPluginDirectory(void)
{
	bool	result = false;

	// find the path <oblivion directory>/data/obse/

	// can't determine how many bytes we'll need, hope it's not more than MAX_PATH
	char	oblivionPathBuf[MAX_PATH];
	UInt32	oblivionPathLength = GetModuleFileName(GetModuleHandle(NULL), oblivionPathBuf, sizeof(oblivionPathBuf));

	if(oblivionPathLength && (oblivionPathLength < sizeof(oblivionPathBuf)))
	{
		std::string	oblivionPath(oblivionPathBuf, oblivionPathLength);

		// truncate at last slash
		std::string::size_type	lastSlash = oblivionPath.rfind('\\');
		if(lastSlash != std::string::npos)	// if we don't find a slash something is VERY WRONG
		{
			oblivionPath = oblivionPath.substr(0, lastSlash + 1);

			_DMESSAGE("oblivion root = %s", oblivionPath.c_str());

			m_pluginDirectory = oblivionPath + "Data\\OBSE\\Plugins\\";
			result = true;
		}
		else
		{
			_WARNING("no slash in oblivion path? (%s)", oblivionPath.c_str());
		}
	}
	else
	{
		_WARNING("couldn't find oblivion path (len = %d, err = %08X)", oblivionPathLength, GetLastError());
	}

	return result;
}

void PluginManager::InstallPlugins(void)
{
	for(IDirectoryIterator iter(m_pluginDirectory.c_str(), "*.dll"); !iter.Done(); iter.Next())
	{
		std::string	pluginPath = iter.GetFullPath();

		_MESSAGE("checking plugin %s", pluginPath.c_str());

		LoadedPlugin	plugin;
		memset(&plugin, 0, sizeof(plugin));

		s_currentLoadingPluginInfo = &plugin.info;

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

	s_currentLoadingPluginInfo = NULL;
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
