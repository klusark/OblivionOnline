#include "Utilities.h"
#include "GameAPI.h"

void PrintItemType(TESForm * form)
{
	Console_Print("%s (%s)", GetFullName(form), GetObjectClassName(form));
}

void DumpClass(void * theClassPtr, UInt32 nIntsToDump)
{
	_MESSAGE("DumpClass:\r\t");
	UInt32* basePtr = (UInt32*)theClassPtr;

	if (!theClassPtr) return;
	for (UInt32 ix = 0; ix < nIntsToDump; ix++ ) {
		UInt32* curPtr = basePtr+ix;
		const char* curPtrName = NULL;
		UInt32 otherPtr = 0;
		float otherFloat = 0.0;
		const char* otherPtrName = NULL;
		if (curPtr) {
			curPtrName = GetObjectClassName((void*)curPtr);
			otherPtr = *curPtr;
			otherFloat = *(float*)(curPtr);
			if (otherPtr) {
				otherPtrName = GetObjectClassName((void*)otherPtr);
			}
		}

		_MESSAGE( "\t%3d +%03X ptr: 0x%08X: %32s *ptr: 0x%08x | %f: %32s\r", ix, ix*4, curPtr, curPtrName, otherPtr, otherFloat, otherPtrName);
	}
}
