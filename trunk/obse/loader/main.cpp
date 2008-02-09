#include <direct.h>
#include "common/IFileStream.h"
#include "Options.h"
#include <string>

IDebugLog	gLog("obse_loader.log");

static bool InjectDLL(PROCESS_INFORMATION * info, const char * dllPath, bool sync = true);
static bool DoInjectDLL(PROCESS_INFORMATION * info, const char * dllPath, bool sync);
static bool TestChecksum(const char * procName, std::string * dllSuffix);
static bool LoadOldblivion(PROCESS_INFORMATION * info);
static void PrintError(const char * fmt, ...);

int main(int argc, char ** argv)
{
	gLog.SetPrintLevel(IDebugLog::kLevel_FatalError);
	gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

	if(!g_options.Read(argc, argv))
	{
		PrintError("Couldn't read arguments.");
		g_options.PrintUsage();

		return -1;
	}

	if(g_options.m_launchCS)
		_MESSAGE("launching editor");

	if(g_options.m_loadOldblivion)
		_MESSAGE("loading oldblivion");

	// create the process
	STARTUPINFO			startupInfo = { 0 };
	PROCESS_INFORMATION	procInfo = { 0 };
	bool				dllHasFullPath = false;

	startupInfo.cb = sizeof(startupInfo);

	const char	* procName = g_options.m_launchCS ? "TESConstructionSet.exe" : "Oblivion.exe";
	const char	* baseDllName = g_options.m_launchCS ? "obse_editor" : "obse";

	if(g_options.m_altEXE.size())
	{
		procName = g_options.m_altEXE.c_str();
		_MESSAGE("launching alternate exe (%s)", procName);
	}

	if(g_options.m_altDLL.size())
	{
		baseDllName = g_options.m_altDLL.c_str();
		_MESSAGE("launching alternate dll (%s)", baseDllName);

		dllHasFullPath = true;
	}

	std::string	dllSuffix;

	if(!TestChecksum(procName, &dllSuffix))
	{
		_ERROR("checksum not found");
		return -1;
	}

	if(g_options.m_crcOnly)
		return 0;

	// build dll path
	std::string	dllPath;
	if(dllHasFullPath)
	{
		dllPath = baseDllName;
	}
	else
	{
		char	currentWorkingDirectory[4096];
		ASSERT(_getcwd(currentWorkingDirectory, sizeof(currentWorkingDirectory)));

		dllPath = std::string(currentWorkingDirectory) + "\\" + baseDllName + "_" + dllSuffix + ".dll";
	}

	_MESSAGE("dll = %s", dllPath.c_str());

	// check to make sure the dll exists
	{
		IFileStream	tempFile;

		if(!tempFile.Open(dllPath.c_str()))
		{
			PrintError("Couldn't find OBSE DLL (%s). Please make sure you have installed OBSE correctly and are running it from your Oblivion folder.", dllPath.c_str());
			return -1;
		}
	}

	bool result = CreateProcess(
		procName,
		NULL,	// no args
		NULL,	// default process security
		NULL,	// default thread security
		TRUE,	// don't inherit handles
		CREATE_SUSPENDED,
		NULL,	// no new environment
		NULL,	// no new cwd
		&startupInfo, &procInfo) != 0;

	ASSERT(result);

	if(g_options.m_setPriority)
	{
		if(!SetPriorityClass(procInfo.hProcess, g_options.m_priority))
			_WARNING("couldn't set process priority");
	}

	result = false;

	if(g_options.m_launchCS)
	{
		// start the process
		ResumeThread(procInfo.hThread);

		// CS needs to run its crt0 code before the DLL is attached, this delays until the message pump is running
		// note that this method makes it impossible to patch the startup code

		// this is better than Sleep(1000) but still ugly
		WaitForInputIdle(procInfo.hProcess, 1000 * 10);

		// too late if this fails
		result = InjectDLL(&procInfo, dllPath.c_str());
		if(!result)
			PrintError("Couldn't inject dll.");
	}
	else
	{
		result = InjectDLL(&procInfo, dllPath.c_str());
		if(result)
		{
			// try to load oldblivion if requested
			if(g_options.m_loadOldblivion)
			{
				result = LoadOldblivion(&procInfo);
				if(!result)
					PrintError("Couldn't load oldblivion.");
			}
		}
		else
			PrintError("Couldn't inject dll.");
		
		if(result)
		{
			_MESSAGE("launching oblivion");

			// start the process
			ResumeThread(procInfo.hThread);
		}
		else
		{
			_ERROR("terminating oblivion process");

			// kill the partially-created process
			TerminateProcess(procInfo.hProcess, 0);
		}
	}

	// clean up
	CloseHandle(procInfo.hProcess);
	CloseHandle(procInfo.hThread);

	return 0;
}

bool InjectDLL(PROCESS_INFORMATION * info, const char * dllPath, bool sync)
{
	bool	result = false;

	// wrap DLL injection in SEH, if it crashes print a message
	__try {
		result = DoInjectDLL(info, dllPath, sync);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		PrintError("DLL injection failed. In most cases, this is caused by an overly paranoid software firewall or antivirus package. Disabling either of these may solve the problem.");
		result = false;
	}

	return result;
}

/*** jmp hook layout
 *	E9 ## ## ## ##	jmp LoadLibraryA
 *						offset = LoadLibraryA - (base + 5)
 *	<string>		name of function
 ***/

bool DoInjectDLL(PROCESS_INFORMATION * info, const char * dllPath, bool sync)
{
	bool	result = false;

	HANDLE	process = OpenProcess(
		PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, info->dwProcessId);
	if(process)
	{
		UInt32	hookBase = (UInt32)VirtualAllocEx(process, NULL, 8192, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if(hookBase)
		{
			UInt32	loadLibraryAAddr = (UInt32)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

			_MESSAGE("hookBase = %08X", hookBase);
			_MESSAGE("loadLibraryAAddr = %08X", loadLibraryAAddr);

			UInt32	bytesWritten;
			WriteProcessMemory(process, (LPVOID)(hookBase + 5), dllPath, strlen(dllPath) + 1, &bytesWritten);

			UInt8	hookCode[5];

			hookCode[0] = 0xE9;
			*((UInt32 *)&hookCode[1]) = loadLibraryAAddr - (hookBase + 5);

			WriteProcessMemory(process, (LPVOID)(hookBase), hookCode, sizeof(hookCode), &bytesWritten);

			HANDLE	thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)hookBase, (void *)(hookBase + 5), 0, NULL);
			if(thread)
			{
				if(sync)
				{
					switch(WaitForSingleObject(thread, g_options.m_threadTimeout))
					{
						case WAIT_OBJECT_0:
							_MESSAGE("hook thread complete");
							result = true;
							break;

						case WAIT_ABANDONED:
							_ERROR("Process::InstallHook: waiting for thread = WAIT_ABANDONED");
							break;

						case WAIT_TIMEOUT:
							_ERROR("Process::InstallHook: waiting for thread = WAIT_TIMEOUT");
							break;
					}
				}
				else
					result = true;

				CloseHandle(thread);
			}
			else
				_ERROR("CreateRemoteThread failed (%d)", GetLastError());

			VirtualFreeEx(process, (LPVOID)hookBase, 8192, MEM_RELEASE);
		}
		else
			_ERROR("Process::InstallHook: couldn't allocate memory in target process");

		CloseHandle(process);
	}
	else
		_ERROR("Process::InstallHook: couldn't get process handle");

	return result;
}

// based on some code from wikipedia
static UInt32 Adler32(UInt8 * buf, UInt32 len)
{
	static const UInt32 kModAdler = 65521;

	UInt32 a = 1, b = 0;

	while (len) {
		unsigned tlen = len > 5550 ? 5550 : len;
		len -= tlen;

		do {
			a += *buf++;
			b += a;
		} while (--tlen);

		a = (a & 0xffff) + (a >> 16) * (65536-kModAdler);
		b = (b & 0xffff) + (b >> 16) * (65536-kModAdler);
	}

	/* It can be shown that a <= 0x1013a here, so a single subtract will do. */
	if (a >= kModAdler) a -= kModAdler;

	/* It can be shown that b can reach 0xffef1 here. */
	b = (b & 0xffff) + (b >> 16) * (65536-kModAdler);
	if (b >= kModAdler) b -= kModAdler;

	return (b << 16) | a;
}

static bool TestChecksum(const char * procName, std::string * dllSuffix)
{
	bool		result = false;
	IFileStream	src;

	if(src.Open(procName))
	{
		UInt8	* buf = new UInt8[src.GetLength()];
		ASSERT(buf);
		src.ReadBuf(buf, src.GetLength());

		// clear "2GB+ address-aware" flag
		// ### this doesn't work correctly for all PE files, but leave it here
		// ### otherwise the CRCs may change
		if(src.GetLength() > 0x13E)
			buf[0x13E] &= ~0x20;

		// *really* clear 2GB+ address-aware flag
		if(src.GetLength() > 0x40)
		{
			UInt32	headerOffset = *((UInt32 *)(buf + 0x3C));
			UInt32	flagsOffset = headerOffset + 0x16;

			if(src.GetLength() > flagsOffset + 2)
			{
				UInt16	* flagsPtr = (UInt16 *)(buf + flagsOffset);

				*flagsPtr &= ~0x0020;
			}
		}

		UInt32	crc = Adler32(buf, src.GetLength());

		delete [] buf;

		// make sure the crc shows up in the console when run with -crconly
		if(g_options.m_crcOnly)
			_FATALERROR("crc = %08X", crc);
		else
			_MESSAGE("crc = %08X", crc);

		if(g_options.m_launchCS)
		{
			switch(crc)
			{
				case 0x96ED4409:	// 1.0.0.303
					*dllSuffix = "1_0";
					result = true;
					break;

				case 0x8F05C3AD:	// 1.2.0.0 - first release, had source control enabled
				case 0xE380C3AF:	// 1.2.0.0 - second release (almost binary-identical)
				case 0x2F9AC10C:	// 1.2.0.404 - third release, again almost binary-identical
					*dllSuffix = "1_2";
					result = true;
					break;

				default:	// unknown checksum
					PrintError("You have an unknown version of the CS. Please check http://obse.silverlock.org to make sure you're using the latest version of OBSE, then send an email to the contact addresses listed in obse_readme.txt if this version is not supported in the latest release. (CRC = %08X)", crc);
					break;
			}
		}
		else
		{
			switch(crc)
			{
				case 0x4998C72A:	// 1.1.0.511 english official
				case 0x8174C8F4:	// 1.1.0.511 english w/ additional patch
				case 0xC857DB78:	// 1.1.0.511 english w/ no-CD patch by TNT
				case 0xE512C810:	// 1.1.0.511 english w/ no-CD patch by SYNapSiS
				case 0x31722297:	// 1.1.0.511 cheesy russian patch (doesn't seem to be official)
				case 0x08FFC506:	// 1.1.0.511 english w/ no-CD patch by cirdan patch #2
				case 0xD240C4E6:	// 1.1.0.511 english w/ no-CD patch
				case 0x9329CBBB:	// 1.1.0.511 english w/ no-CD patch
				case 0x278B2356:	// 1.1.0.511 japanese unofficial v5
				case 0xAC232487:	// 1.1.0.511 polish unofficial
				case 0x29F82D28:	// 1.1.0.511 russian unofficial w/ no-CD "<B@ZiK was here!>"
				case 0x7F601478:	// 1.1.0.511 japanese unofficial v6
				case 0x5D4D091D:	// 1.1.0.511 japanese unofficial v7c
				case 0x9F9F923B:	// 1.1.0.511 japanese unofficial v9 (apparently no v8)
				case 0x292A8DA6:	// 1.1.0.511 japanese unofficial v9a
				case 0x0ACF90F4:	// 1.1.0.511 japanese unofficial v9b
				case 0x344D33A1:	// 1.1.0.511 japanese unofficial v10
					*dllSuffix = "1_1";
					result = true;
					break;

				case 0x3F6987B1:	// 1.1.0.425 beta patch
					PrintError("You are using the 1.1.0.425 beta patch of Oblivion, please upgrade to 1.2.0.416.");
					break;

				case 0x92FE2FC9:	// 0.1.0.228 original w/ no-CD patch by *!ReLOADeD!*
				case 0x01D42F77:	// 0.1.0.228 original
				case 0x2CDBAC93:	// 0.1.0.228 japanese unofficial v7c
				case 0xB9572CEF:	// 0.1.0.228 original w/ no-CD patch by cirdan
					PrintError("You are using the original 0.1.0.228 version of Oblivion, please upgrade to 1.2.0.416.");
					break;

				case 0x608CA512:	// 1.1.0.511 english direct2drive
				case 0xAE9944C0:	// 1.2.something english direct2drive
					PrintError("Direct2drive versions of Oblivion are not supported due to copy protection applied to the executable.");
					break;

				case 0xACB74E24:	// 1.2.0.214 english official - installed by the 1.2 patch
				case 0x46A74FEE:	// 1.2.0.214 english w/ no-CD patch by unknown
				case 0x10D34E25:	// 1.2.0.214 english w/ no-CD patch by refraction
				case 0xBF924BD4:	// 1.2.0.214 english w/ no-CD patch by unknown
				case 0x33F353A2:	// 1.2.0.214 english w/ no-CD patch by unleashed
				case 0xBD765745:	// 1.2.0.214 english w/ no-CD patch by TNT
				case 0xE116E23D:	// 1.2.0.214 japanese unofficial v7c
				case 0x78AEE28E:	// 1.2.0.214 japanese unofficial v7d (hello 2ch)
				case 0x960EE2AE:	// 1.2.0.214 japanese unofficial v7e
				case 0x7B5ED597:	// 1.2.0.214 polish unofficial
					*dllSuffix = "1_2";
					result = true;
					break;

				case 0xE8B71FA4:	// 1.2.0.201 english official - installed by Shivering Isles
				case 0xF6E56D02:	// 1.2.0.201 w/ no-CD patch
					PrintError("You are using the 1.2.0.201 version of Oblivion installed by Shivering Isles, please upgrade to 1.2.0.416.");
					break;

				case 0x6C9D751B:	// 1.2.0.410 beta patch (bugfix for refid generation)
				case 0xE17F0878:	// 1.2.0.410 japanese unofficial v7d
				case 0xFEDF0898:	// 1.2.0.410 japanese unofficial v7e
					PrintError("You are using the 1.2.0.410 beta patch, which is not supported. There is no way to directly patch from 1.2.0.410 to any other version of Oblivion, so please check the readme for instructions on using the autopatcher to upgrade to 1.2.0.416.");
					break;

				case 0x7934C86C:	// 1.2.0.416 patch - released as both beta and final
				case 0x3E51C65C:	// 1.2.0.416 w/ no-CD patch by pwz "osi6"
				case 0x9339C8B3:	// 1.2.0.416 w/ no-CD patch by pwz "si46"
				case 0xE7BBCA0E:	// 1.2.0.416 w/ no-CD patch by pwz "si40" - dude, it's the SAME BASE EXE, WHY CRACK IT DIFFERENTLY THREE TIMES?
				case 0x3ECC5E59:	// 1.2.0.416 japanese unofficial v7e
				case 0x3D4EC785:	// 1.2.0.416 german unofficial
				case 0x2B4DD302:	// 1.2.0.416 russian official?
				case 0xD90B4B42:	// 1.2.0.416 polish
				case 0x181B8C89:	// 1.2.0.416 japanese unofficial v9 (apparently no v8)
				case 0xE56C9816:	// 1.2.0.416 japanese unofficial v9a
				case 0x3E9FC1F7:	// 1.2.0.416 japanese unofficial v9b
				case 0xC9A9C9AD:	// 1,2.0.416 w/ no-CD patch by unknown
				case 0x52AE527E:	// 1.2.0.416 japanese unofficial v10
				case 0xABD5C927:	// 1.2.0.416 german unofficial
				case 0x97D6D510:	// 1.2.0.416 russian official? http://games.1c.ru/oblivion_gold/
				case 0xAD9CD911:	// 1.2.0.416 russian official + no-CD
					*dllSuffix = "1_2_416";
					result = true;
					break;

				default:	// unknown checksum
					PrintError("You have an unknown version of Oblivion. Please check http://obse.silverlock.org to make sure you're using the latest version of OBSE, then send an email to the contact addresses listed in obse_readme.txt if this version is not supported in the latest release. (CRC = %08X)", crc);
					break;
			}
		}
	}
	else
	{
		PrintError("Couldn't find %s, make sure you're running this from the same folder as your oblivion.exe.", procName);
	}

	return result;
}

static bool LoadOldblivion(PROCESS_INFORMATION * info)
{
	bool	result = false;
	HANDLE	syncEvent = CreateEvent(NULL, 0, 0, "OldblivionInjectionCompleted");

	if(syncEvent)
	{
		if(InjectDLL(info, "oldblivion.dll", false))
		{
			switch(WaitForSingleObject(syncEvent, g_options.m_threadTimeout))
			{
				case WAIT_OBJECT_0:
					_MESSAGE("oldblivion loaded");
					result = true;
					break;

				case WAIT_ABANDONED:
					_ERROR("LoadOldblivion: waiting for oldblivion = WAIT_ABANDONED");
					break;

				case WAIT_TIMEOUT:
					_ERROR("LoadOldblivion: waiting for oldblivion = WAIT_TIMEOUT");
					break;
			}
		}
		else
			_ERROR("couldn't inject oldblivion dll");

		CloseHandle(syncEvent);
	}
	else
		_ERROR("couldn't create oldblivion sync event (%d)", GetLastError());

	return result;
}

static void PrintError(const char * fmt, ...)
{
	va_list	args;

	va_start(args, fmt);

	gLog.Log(IDebugLog::kLevel_FatalError, fmt, args);

	char	buf[4096];
	vsprintf_s(buf, sizeof(buf), fmt, args);

	MessageBox(NULL, buf, "OBSE Loader", MB_OK | MB_ICONEXCLAMATION);

	va_end(args);
}
