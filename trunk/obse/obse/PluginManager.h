#pragma once

#include <string>
#include <list>

#include "obse/PluginAPI.h"

class PluginManager
{
public:
	PluginManager();
	~PluginManager();

	bool	Init(void);
	void	DeInit(void);

	PluginInfo *	GetInfoByName(const char * name);

private:
	bool	FindPluginDirectory(void);
	void	InstallPlugins(void);

	struct LoadedPlugin
	{
		HMODULE		handle;
		PluginInfo	info;

		_OBSEPlugin_Query	query;
		_OBSEPlugin_Load	load;
	};

	typedef std::list <LoadedPlugin>	LoadedPluginList;

	std::string			m_pluginDirectory;
	LoadedPluginList	m_plugins;
};

extern PluginManager	g_pluginManager;

extern CommandInfo kCommandInfo_IsPluginInstalled;
extern CommandInfo kCommandInfo_GetPluginVersion;
