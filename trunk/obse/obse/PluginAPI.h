#pragma once

struct CommandInfo;

typedef UInt32	PluginHandle;	// treat this as an opaque type

enum
{
	kPluginHandle_Invalid = 0xFFFFFFFF
};

enum
{
	kInterface_Console = 0,

	// added in v0015
	kInterface_Serialization,

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

	// added in v0015, only call if obseVersion >= 15
	// call during your Query or Load functions to get a PluginHandle uniquely identifying your plugin
	// invalid if called at any other time, so call it once and save the result
	PluginHandle	(* GetPluginHandle)(void);
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

/**** serialization API docs ***************************************************
 *	
 *	The serialization API adds a separate save data file plugins can use to save
 *	data along with a game save. It is be stored separately from the main save
 *	(.ess) file to prevent any potential compatibility issues. The actual
 *	implementation is opaqe, so it can be changed if needed, but the APIs
 *	provided will always remain the same and will operate the same way.
 *	
 *	Each plugin that has registered the proper callbacks will be able to write
 *	typed and versioned records in to the file, similarly to the way Oblivion
 *	.esp files work. Chunk types are just simple 32-bit values, you can use
 *	them for whatever you want as they are only meaningful to your code.
 *	Multiple records of the same type can be added, and records will be
 *	returned to you in the order they were originally written. Versioning is
 *	important so you can update the format of your data without breaking
 *	people's save files. Note that this means that if you have created multiple
 *	versions of a record you will have to check the version number and do some
 *	data conversion on load. Of course it isn't strictly /mandatory/ that you do
 *	this, but I consider not breaking people's save files to be very important.
 *	Also, note that your record data will be uniquely identified by your
 *	assigned opcode base, so make sure that is set up correctly (you really
 *	have to be doing that anyway, but I thought I'd mention it again).
 *	
 *	At any point, a plugin can call the
 *	OBSEStorageInterface::SetSave/Load/NewGameCallback functions to register a
 *	callback that will be run when a game is being saved, loaded, or started
 *	fresh. You must provide the PluginHandle you were given during your startup
 *	function so the core code knows which plugin to associate with the callback.
 *	
 *	When the save callback is called, there are two APIs you can use to write
 *	your records. WriteRecord is the simplest API - just give it your record
 *	type, the version, and a buffer storing your data. This is good when you can
 *	write your entire record all at one time. If you have more complex needs,
 *	use the OpenRecord/WriteRecordData APIs. When starting to write a new
 *	record, call OpenRecord, passing in the record type and version. Then call
 *	WriteRecordData as many times as needed to fill in the data for the record.
 *	To start the next record, just call OpenRecord again. Calling OpenRecord or
 *	exiting your save callback will automatically close the record and perform
 *	behind-the-scenes cleanup.
 *	
 *	The load callback is simpler. First call GetNextRecordInfo. It will move to
 *	the next record (or the first record if it is the first time it has been
 *	called) and return information about it. GetNextRecordInfo returns true if
 *	it found a "next" record for your plugin, so you know you are done reading
 *	when it returns false. After calling GetNextRecordInfo, call ReadRecordData
 *	to retrieve the data stored in the record. It may be called multiple times,
 *	and returns the number of bytes read from the record (really only useful if
 *	you accidentally try to read off the end of the record).
 *	
 *	The new game callback should be used to reset all of your internal data
 *	structures. It is called when a new game is started or a game is loaded with
 *	no save file.
 *	
 *	RefIDs stored in this file involve one complication. The upper 8 bits of the
 *	ID store the index of the mod that "owns" the form. This index may change in
 *	between save and load if a user changes their mod list. To fix this, run the
 *	ID through the ResolveRefID API. It fixes up the ID to reflect any changes
 *	in the mod list. If the mod owning that ID is no longer loaded, the function
 *	returns false.
 *	
 ******************************************************************************/

struct OBSESerializationInterface
{
	enum
	{
		kVersion = 1,
	};

	typedef void (* EventCallback)(void * reserved);

	UInt32	version;
	void	(* SetSaveCallback)(PluginHandle plugin, EventCallback callback);
	void	(* SetLoadCallback)(PluginHandle plugin, EventCallback callback);
	void	(* SetNewGameCallback)(PluginHandle plugin, EventCallback callback);

	bool	(* WriteRecord)(UInt32 type, UInt32 version, const void * buf, UInt32 length);
	bool	(* OpenRecord)(UInt32 type, UInt32 version);
	bool	(* WriteRecordData)(const void * buf, UInt32 length);

	bool	(* GetNextRecordInfo)(UInt32 * type, UInt32 * version, UInt32 * length);
	UInt32	(* ReadRecordData)(void * buf, UInt32 length);

	// take a refid as stored in the loaded save file and resolve it using the currently
	// loaded list of mods. All refids stored in a save file must be run through this
	// function to account for changing mod lists. This returns true on success, and false
	// if the mod owning the RefID was unloaded.
	bool	(* ResolveRefID)(UInt32 refID, UInt32 * outRefID);
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
 *	Note that all structure versions are backwards-compatible, so you only need
 *	to check against the latest version that you need. New fields will be only
 *	added to the end, and all old fields will remain compatible with their
 *	previous implementations.
 *	
 ******************************************************************************/
