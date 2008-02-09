#include <stdio.h>
#include <string.h>
#include "Commands_Console.h"
#include "ParamInfos.h"

#ifdef OBLIVION

#include "GameAPI.h"
#include "GameForms.h"
#include "GameObjects.h"

/* Print formatted string to Oblivion console
 * syntax: PrintToConsole fmtstring num1 num2 ...
 * shortname: printc
 *
 * Prints a formatted string to the Oblivion console, similar to how printf() works.
 * -Format notation is the same as MessageBox, so you can use %#.#f %g %e %%
 * -While this function technically accepts floats, you can use %g to print integers.  Again, how this works
 *  is probably similar to how MessageBox works (ie. same range limitations).
 * -The string can be up to 511 characters long, not including the null byte.
 * -You can pass up to 9 variables, but you don't have to pass any at all.
 *
 * Updated v0014: Takes additional format specifiers as MessageEX/MessageBoxEX
 * retains previous functionality
 */
bool Cmd_PrintToConsole_Execute(COMMAND_ARGS)
{
	*result = 0;
	char buffer[512];

	if (ExtractFormattedString(buffer, arg1, scriptObj, eventList, opcodeOffsetPtr))
	{
		*result = 1;
		Console_Print(buffer);
	}

	return true;
}

/* Print formatted string to the given file
 * syntax:  PrintToFile fmtstring num1 num1 ...
 * shortname: printf
 *
 * Prints a formatted string to the given file, similar to using PrintToConsole.
 * -You can use integers here like with PrintToConsole and MessageBox.
 * -The string can be up to 511 character long, not including the null byte.
 * -You can pass up to 9 variables, but you don't have to pass any at all.
 * -If the file does not exist, it will be created.
 * -This function will always append to the end of a file on a new line.
 */
bool Cmd_PrintToFile_Execute(COMMAND_ARGS)
{
	char filename[129], fmtstring[BUFSIZ];
	FILE* fileptr;
	float f0, f1, f2, f3, f4, f5, f6, f7, f8;

	if (ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &filename, &fmtstring, &f0, &f1, &f2, &f3, &f4,
					&f5, &f6, &f7, &f8))
	{
		fopen_s (&fileptr, filename, "a+");
		strcat_s(fmtstring, sizeof(fmtstring), "\n");
		fprintf (fileptr, fmtstring, f0, f1, f2, f3, f4, f5, f6, f7, f8);
		fclose (fileptr);
	}
	return true;
}

void RunScriptLine(const char * buf)
{
	// create a Script object
	UInt8	scriptObjBuf[sizeof(Script)];
	Script	* tempScriptObj = (Script *)scriptObjBuf;

	void	* scriptState = GetGlobalScriptStateObj();

	tempScriptObj->Constructor();
	tempScriptObj->MarkAsTemporary();
	tempScriptObj->SetText(buf);
	tempScriptObj->CompileAndRun(*((void **)scriptState), 1, NULL);
	tempScriptObj->StaticDestructor();
}

bool Cmd_RunBatchScript_Execute(COMMAND_ARGS)
{
	char	fileName[1024];

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &fileName)) return true;

	FILE	* src = NULL;
	fopen_s(&src, fileName, "r");
	if(src)
	{
		char	line[4096];
		while(fgets(line, sizeof(line), src))
		{
			UInt32	lineLen = strlen(line);
			if(lineLen > 1)	// skip empty lines
			{
				// strip the trailing newline (if we have one)
				if(line[lineLen - 1] == '\n')
					line[lineLen - 1] = 0;

				RunScriptLine(line);
			}
		}

		fclose(src);
	}

	return true;
}

#endif

ParamInfo kParams_StringFormat[10] =
{
	{"string", kParamType_String, 0},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1}
};

ParamInfo kParams_StringFormatFile[11] =
{
	{"string", kParamType_String, 0},
	{"string", kParamType_String, 0},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1}
};

static ParamInfo kParams_Message[21] =
{
	{"format string",	kParamType_String, 0},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
};

CommandInfo kCommandInfo_PrintToConsole =
{
	"PrintToConsole",
	"printc",
	0,
	"Print formatted string to console",
	0,
	21,
	kParams_Message,
	HANDLER(Cmd_PrintToConsole_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_PrintToFile =
{
	"PrintToFile",
	"printf",
	0,
	"Append formatted string to file",
	0,
	11,
	kParams_StringFormatFile,
	HANDLER(Cmd_PrintToFile_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_RunBatchScript =
{
	"RunBatchScript",
	"",
	0,
	"Similar to the \'bat\' console command",
	0,
	1,
	kParams_OneString,
	HANDLER(Cmd_RunBatchScript_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

