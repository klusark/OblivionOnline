#pragma once

struct CommandInfo;

enum
{
	kInterface_Console = 0,

	kInterface_Max
};

struct OBSEInterface
{
	UInt32	obseVersion;
	UInt32	oblivionVersion;
	UInt32	editorVersion;
	UInt32	isEditor;
	bool	(* RegisterCommand)(CommandInfo * info);	// returns true for success, false for failure
	void	(* SetOpcodeBase)(UInt32 opcode);
	void *	(* QueryInterface)(UInt32 id);
};

struct OBSEConsoleInterface
{
	enum
	{
		kVersion = 1
	};

	UInt32	version;
	void	(* RunScriptLine)(const char * buf);
};

struct PluginInfo
{
	enum
	{
		kInfoVersion = 1
	};

	UInt32			infoVersion;
	const char *	name;
	UInt32			version;
};

typedef bool (* _OBSEPlugin_Query)(const OBSEInterface * obse, PluginInfo * info);
typedef bool (* _OBSEPlugin_Load)(const OBSEInterface * obse);

/**** plugin API docs **********************************************************
 *	
 *	IMPORTANT: Before releasing a plugin, you MUST contact the OBSE team at the
 *	contact addresses listed in obse_readme.txt to register a range of opcodes.
 *	This is required to prevent conflicts between multiple plugins, as each
 *	command must be assigned a unique opcode.
 *	
 *	The base API is pretty simple. Create a project based on the
 *	obse_plugin_example project included with the OBSE source code, then define
 *	and export these functions:
 *	
 *	bool OBSEPlugin_Query(const OBSEInterface * obse, PluginInfo * info)
 *	
 *	This primary purposes of this function are to fill out the PluginInfo
 *	structure, and to perform basic version checks based on the info in the
 *	OBSEInterface structure. Return false if your plugin is incompatible with
 *	the version of OBSE or Oblivion passed in, otherwise return true. In either
 *	case, fill out the PluginInfo structure.
 *	
 *	If the plugin is being loaded in the context of the editor, isEditor will be
 *	non-zero, editorVersion will contain the current editor version, and
 *	oblivionVersion will be zero. In this case you can probably just return
 *	true, however if you have multiple DLLs implementing the same behavior, for
 *	example one for each version of Oblivion, only one of them should return
 *	true.
 *	
 *	The PluginInfo fields should be filled out as follows:
 *	- infoVersion should be set to PluginInfo::kInfoVersion
 *	- name should be a pointer to a null-terminated string uniquely identifying
 *	  your plugin, it will be used in the plugin querying API
 *	- version is only used by the plugin query API, and will be returned to
 *	  scripts requesting the current version of your plugin
 *	
 *	bool OBSEPlugin_Load(const OBSEInterface * obse)
 *	
 *	In this function, use the SetOpcodeBase callback in OBSEInterface to set the
 *	opcode base to your assigned value, then use RegisterCommand to register all
 *	of your commands. OBSE will fix up your CommandInfo structure when loaded
 *	in the context of the editor, and will fill in any NULL callbacks with their
 *	default values, so don't worry about having a unique 'execute' callback for
 *	the editor, and don't provide a 'parse' callback unless you're actually
 *	overriding the default behavior. The opcode field will also be automatically
 *	updated with the next opcode in the sequence started by SetOpcodeBase.
 *	
 *	At this time, or at any point forward you can call the QueryInterface
 *	callback to retrieve an interface structure for the base services provided
 *	by the OBSE core.
 *	
 *	You may optionally return false from this function to unload your plugin,
 *	but make sure that you DO NOT register any commands if you do.
 *	
 ******************************************************************************/
