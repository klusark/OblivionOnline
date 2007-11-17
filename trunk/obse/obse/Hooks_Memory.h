#pragma once

#include "CommandTable.h"

void Hook_Memory_Init(void);
void Hook_Memory_DeInit(void);

UInt32 GetPoolAllocationSize(void * buf);

extern CommandInfo	kCommandInfo_DebugMemDump;
