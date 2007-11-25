Oblivion Script Extender v0013
by Ian Patterson and Stephen Abel and Paul Connolly
(ianpatt, behippo and scruggsywuggsy the ferret)

The Oblivion Script Extender, or OBSE for short, is a modder's resource that expands the scripting capabilities of Oblivion. It doesn't make any modifications to oblivion.exe, TESConstructionSet.exe, or any other files in your oblivion install, so don't worry about permanent side effects. It is compatible with the 1.2.0.416, 1.2.0.214, and 1.1.0.511 Oblivion patches, as well as the 1.0 and 1.2 versions of the Construction Set.

This release adds:
- climate manipulation functions
- new easier-to-use input functions
- active magic effect manipulation and querying functions
- plugin querying API
- cell water level functions
- leveled list functions
- weather manipulation functions
- get/set travel horse
- get/set open key
- get creature reach, soul level and base scale
- set/copy hair and eyes (changes not saved on NPCs)
- get owner, get owning faction required rank
- get parent cell owner, get parent cell owner faction required rank
- get/set container respawns (changes not saved)
- IsLoadDoor, GetLinkedDoor, GetTeleportCell
- RefreshControlMap
- GetFirstRef, GetNextRef, GetNumRefs

[ Installation ]

1. Copy obse_1_1.dll, obse_1_2.dll, obse_1_2_416.dll, obse_editor_1_0.dll, obse_editor_1_2.dll, and obse_loader.exe to your Oblivion directory.
2. Run oblivion by running obse_loader.exe from the Oblivion directory.

Scripts written with these new commands must be written via the TESConstructionSet launched with obse_loader. Open a command prompt window, navigate to your oblivion install folder, and type "obse_loader -editor". The normal editor can open plugins with these extended scripts fine, it just can't recompile them and will give errors if you try.

[ Suggestions for Modders ]

If your mod requires OBSE, please provide a link to the main OBSE website, http://obse.silverlock.org/ instead of packaging it with your mod install. Future versions of OBSE will be backwards compatible, so including a potentially old version can cause confusion and/or break other mods that need newer versions. If you're making a large mod with an installer, inclusion of a specific version of OBSE is OK, but please check the file versions of the OBSE files before replacing them, and only replace earlier versions.

When your mod loads, use the command GetOBSEVersion to make sure a compatible version of OBSE is installed. In general, make sure you're testing for any version later than the minimum version you support, as each update to OBSE will have a higher version number. Something like:

if GetOBSEVersion < 5 then
  MessageBox "This mod requires a newer version of OBSE."
endif

Another alternative that is even more user-friendly is releasing your mod as an OMOD, and adding a script to check the version when installing the mod:

IfNot ScriptExtenderNewerThan #.#.#.#
    Message "Requires Oblivion Script Extender v#### or greater"
    FatalError
EndIf

[ Troubleshooting / FAQ ]

Oblivion doesn't launch after running obse_loader.exe:

- Make sure you've copied the OBSE files to your oblivion folder. That folder should also contain oblivion.exe.
- Check the file obse_loader.log in your oblivion folder for errors.

obse_loader.log tells me it couldn't find a checksum:

- You may have a version of Oblivion that isn't supported. I have the english official patch v1.2.0.416.  Localized versions with different executables or different patches may not work, but many have been successful. If there's enough legitimate demand for it, I can add support for other versions in the future.
- Your Oblivion install may be corrupt. Hacks or no-cd patches may also change the checksum of the game, making it impossible to detect the installed version.

The OBSE loader tells me I need to use the autopatcher:

- Go to to OBSE website (http://obse.silverlock.org) and download autopatcher, which will walk you through the update process. You will need the latest patch from Bethesda, as well as your original Oblivion DVD.

OBSE doesn't launch with the Direct2Drive version:

- The Direct2Drive version of the Oblivion executable is compressed and possibly encrypted. This makes it very difficult to support. Whether or not it is supportable in the future is unknown. Apologies.

Crashes or other strange behavior:

- Let me know how you made it crash, and I'll see about fixing it.

Xbox 360 or PS3 version?

- Impossible.

Running OBSE and Oldblivion at the same time:

- Copy your oldblivion support files in to the Oblivion folder (oldblivion.dll, oldblivion.cfg, shaders.sdp), then run the loader with the -old command argument.

I'm using Windows 2000 and some mods (specifically mods using IsKeyPressed) don't work correctly. What should I do?

- Open your Oblivion.ini (found in My Documents/My Games/Oblivion) and change the "bBackground Keyboard=0" line to "bBackground Keyboard=1".

How do I change the script editor font?

- Hold F12 or F12 while opening the script editor. F12 will default to Lucida Console 9pt, and F11 will show a font picker dialog box.

Can I modify and release my own version of OBSE based on the released source code?

- This is highly discouraged. Each command must be assigned a unique and constant opcode from 0x1000-0x7FFF. Bethesda started adding commands at 0x1000, and OBSE started adding at 0x1400. If you add new commands yourself, they may conflict with later releases of OBSE. The suggested method for extending OBSE is to write a plugin. If this does not meet your needs, please email the contact addresses listed below.

How do I write a plugin for OBSE?

- Start with the obse_plugin_example project in the OBSE source distribution. Currently the documentation for the plugin API can be found in the source distribution under obse/obse/PluginAPI.h. Note that due to the opcode allocation issues discussed above, you will need to request an opcode range for your plugin by emailing the contact addresses at the bottom of the readme. Also note that plugins must have their source code available. The OBSE team has spent a very long time developing and documenting the interface to Oblivion's internals, so we request that plugin developers also share their findings with the public. This will also help us make sure that we keep Oblivion as stable as possible even when using third-party plugins.

What is the 'src' folder for?

- The 'src' folder contains the source code for OBSE. Feel free to delete or ignore it if you are not a programmer interested in contributing to the project.

[ Contact the OBSE Team ]

Ian (ianpatt) - Designer and lead programmer
Send email to ianpatt+obse [at] gmail [dot] com. (yes, keep the + symbol, it helps me sort email)

Stephen (behippo)
Send email to gamer [at] silverlock [dot] org
