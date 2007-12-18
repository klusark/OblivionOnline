#pragma once

#include "CommandTable.h"

extern CommandInfo kCommandInfo_PrintToConsole;
extern CommandInfo kCommandInfo_PrintToFile;
extern CommandInfo kCommandInfo_RunBatchScript;
extern CommandInfo kCommandInfo_GetFormID;

void RunScriptLine(const char * buf);
