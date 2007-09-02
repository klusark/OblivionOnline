#include "Hooks_Memory.h"
#include "GameAPI.h"
#include "obse_common/SafeWrite.h"
#include "common/ICriticalSection.h"
#include <share.h>
#include <set>

// enable everything
#define ENABLE_MEMORY_DEBUG 0 && _DEBUG

// clear new allocs to CDCDCDCD and freed buffers to DEDEDEDE
#define CLEAR_MEMORY 1

// write allocations and deallcations to a log file
#define LOG_MEMORY 1

// keep track of allocations, checking for double-frees or assignment to the same address twice
#define TRACK_ALLOCATIONS 1

class MemoryHeap
{
public:
	MemoryHeap(UInt32 mainSize, UInt32 unk1);
	
	void *	Allocate(UInt32 size, UInt32 unk);
	void	Free(void * ptr);

	void	CreatePool(UInt32 entrySize, UInt32 totalSize);

	bool	IsMainHeapAllocation(void * ptr);
	
	virtual void	Unk_0(UInt32 unk0);
	virtual void *	AllocatePage(UInt32 size);
	virtual void *	RawAlloc(UInt32 unk0);
	virtual void *	RawAlloc2(UInt32 unk0);
	virtual void	FreeMemory(void * buf);
	virtual void	RawFree(void * buf);
	virtual void	RawFree2(void * buf);
	virtual UInt32	RawSize(void * buf);
	
	// memory panic callback
	typedef void (* Unk164Callback)(UInt32 unk0, UInt32 unk1, UInt32 unk2);
	
//	void	** _vtbl;	// 000
	UInt32	field_004;	// 004 - alignment
	UInt32	field_008;	// 008
	UInt32	field_00C;	// 00C - size of main memory block
	UInt32	field_010;	// 010
	UInt32	field_014;	// 014
	void	* field_018;	// 018 - main memory block
	UInt32	field_01C;	// 01C
	UInt32	field_020;	// 020
	UInt32	field_024;	// 024
	UInt32	field_028;	// 028
	UInt32	field_02C;	// 02C
	UInt32	field_030;	// 030 - size of field_034 / 8
	void	* field_034;	// 034 - 0x2000 byte buffer
	void	* field_038;	// 038 - end of field_034
	UInt32	field_03C;	// 03C
	UInt32	field_040;	// 040
	void	* field_044;	// 044
	UInt32	field_048;	// 048
	UInt32	field_04C;	// 04C
	UInt32	field_050;	// 050
	UInt32	field_054;	// 054 - available memory at startup
	UInt32	field_058;	// 058
	UInt32	field_05C;	// 05C
	UInt32	field_060;	// 060
	UInt32	field_064;	// 064
	UInt32	unk_068[(0x164 - 0x068) >> 2];	// 068
	Unk164Callback	field_164;	// 164
	UInt32	field_168;	// 168 - used memory at startup
	UInt8	field_16C;	// 16C
	UInt8	field_16D;	// 16D
	// 16E
};

STATIC_ASSERT(offsetof(MemoryHeap, field_16D) == 0x16D);

bool MemoryHeap::IsMainHeapAllocation(void * ptr)
{
	UInt8	* mainHeap = (UInt8 *)g_formHeap->field_018;
	UInt8	* mainHeapEnd = mainHeap + g_formHeap->field_00C;
	UInt8	* result8 = (UInt8 *)ptr;

	return mainHeap && ((result8 >= mainHeap) && (result8 < mainHeapEnd));
}

class MemoryPool
{
public:
	MemoryPool(UInt32 entrySize, UInt32 totalSize, const char * name);
	~MemoryPool();
	
	void *	Allocate(void);
	
	char	m_name[0x40];	// 000
	void	* field_040;	// 040 - base buffer
	UInt32	field_044;	// 044

	UInt32	unk_048[(0x080 - 0x048) >> 2];	// 048

	CRITICAL_SECTION	critSection;	// 080

	UInt32	unk_098[(0x100 - 0x098) >> 2];	// 098

	UInt32	field_100;	// 100 - entry size
	UInt32	field_104;	// 104
	UInt16	* field_108;	// 108 - page allocation count (FFFF - unallocated)
	UInt32	field_10C;	// 10C - size of field_108 (in UInt16s)
	UInt32	field_110;	// 110 - total size
	UInt32	field_114;	// 114
	UInt32	field_118;	// 118
	// 11C
};

STATIC_ASSERT(offsetof(MemoryPool, field_118) == 0x118);

MemoryPool **	g_memoryHeap_poolsBySize = (MemoryPool **)0x00B33080;		// size = 0x81
MemoryPool **	g_memoryHeap_poolsByAddress = (MemoryPool **)0x00B32C80;	// size = 0x100

static const UInt32 kMemoryHeap_Allocate_Addr = 0x00401AA7;
static const UInt32 kMemoryHeap_Free_Addr = 0x00401D46;

static UInt32 g_heapAllocTotal = 0;		// total number of heap allocations
static UInt32 g_heapAllocCurrent = 0;	// number of non-freed heap allocations

#if TRACK_ALLOCATIONS
typedef std::map <void *, UInt32>	AllocationInfoMap;

static AllocationInfoMap	g_allocationInfoMap;
static ICriticalSection		g_allocationInfoMapLock;
#endif

#pragma optimize("", off)

__declspec(naked) void * MemoryHeap::Allocate(UInt32 size, UInt32 unk)
{
	__asm
	{
		push	esi
		mov		esi, ecx
		cmp		dword ptr [esi+0x0C], 0
		jmp		[kMemoryHeap_Allocate_Addr]
	}
}

__declspec(naked) void MemoryHeap::Free(void * ptr)
{
	__asm
	{
		push	esi
		push	edi
		mov		edi, [esp+0x0C]
		jmp		[kMemoryHeap_Free_Addr]
	}
}

FILE	* memLog = NULL;

#pragma warning (disable : 4313)

static void * __stdcall MemoryHeap_Allocate_Hook(UInt32 size, UInt32 unk)
{
	void	* result;
	
	result = g_formHeap->Allocate(size, unk);

	if(g_formHeap->IsMainHeapAllocation(result))
	{
		g_heapAllocTotal++;
		g_heapAllocCurrent++;

#if LOG_MEMORY
		fprintf(memLog, "a %08X %08X %08X\n", result, size, unk);

		if((g_heapAllocTotal % 10000) == 0)
		{
			fprintf(memLog, "alloc count = %d\n", g_heapAllocTotal);
			fflush(memLog);
		}
#endif

#if TRACK_ALLOCATIONS
		{
			g_allocationInfoMapLock.Enter();

			AllocationInfoMap::iterator	iter = g_allocationInfoMap.find(result);
			if(iter == g_allocationInfoMap.end())
			{
				g_allocationInfoMap[result] = size;
			}
			else
			{
				fprintf(memLog, "### double allocation at address %08X (old size %08X new size %08X)\n",
					result, g_allocationInfoMap[result], size);
				fflush(memLog);
			}

			g_allocationInfoMapLock.Leave();
		}
#endif
	}

#if CLEAR_MEMORY
	if(result)
	{
		memset(result, 0xCDCDCDCD, size);
	}
#endif

	return result;
}

static void __stdcall MemoryHeap_Free_Hook(void * ptr)
{
	if(g_formHeap->IsMainHeapAllocation(ptr))
	{
#if LOG_MEMORY
		fprintf(memLog, "d %08X\n", ptr);
#endif

		g_heapAllocCurrent--;
		if(g_heapAllocCurrent == 0xFFFFFFFF)
		{
			fprintf(memLog, "### double free\n", ptr);
			fflush(memLog);

			g_heapAllocCurrent = 0;
		}

#if TRACK_ALLOCATIONS
		{
			g_allocationInfoMapLock.Enter();

			AllocationInfoMap::iterator	iter = g_allocationInfoMap.find(ptr);
			if(iter == g_allocationInfoMap.end())
			{
				fprintf(memLog, "### bad free of %08X\n", ptr);
				fflush(memLog);
			}
			else
			{
				g_allocationInfoMap.erase(iter);
			}

			g_allocationInfoMapLock.Leave();
		}
#endif
	}

	if(ptr)
	{
#if CLEAR_MEMORY
		UInt32		ptr32 = (UInt32)ptr;
		MemoryPool	* pool = g_memoryHeap_poolsByAddress[ptr32 >> 24];

		if(pool)
		{
			memset(ptr, 0xDEDEDEDE, pool->field_100);
		}
#endif

		g_formHeap->Free(ptr);
	}
}

#pragma optimize("", on)

void Hook_Memory_Init(void)
{
#if ENABLE_MEMORY_DEBUG

#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	WriteRelJump(0x00401AA0, (UInt32)MemoryHeap_Allocate_Hook);
	WriteRelJump(0x00401D40, (UInt32)MemoryHeap_Free_Hook);
#endif

	memLog = _fsopen("h:\\memlog.txt", "w", _SH_DENYWR);

#endif
}

void Hook_Memory_DeInit()
{
#if ENABLE_MEMORY_DEBUG

#if TRACK_ALLOCATIONS
	if(!g_allocationInfoMap.empty())
	{
		fprintf(memLog, "%d leaked allocations\n", g_allocationInfoMap.size());

		for(AllocationInfoMap::iterator iter = g_allocationInfoMap.begin(); iter != g_allocationInfoMap.end(); ++iter)
		{
			fprintf(memLog, "@%08X size %08X\n", iter->first, iter->second);
		}
	}
#endif

	fprintf(memLog, "alloc count = %d\n", g_heapAllocTotal);

	fclose(memLog);
	memLog = NULL;

#endif
}

UInt32 GetPoolAllocationSize(void * buf)
{
	UInt32	result = 0;
	UInt32	buf32 = (UInt32)buf;

	MemoryPool	* pool = g_memoryHeap_poolsByAddress[buf32 >> 24];
	if(pool)
		result = pool->field_100;

	return result;
}

bool Cmd_DebugMemDump_Execute(COMMAND_ARGS)
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	for(UInt32 i = 0; i <= 0x81; i++)
	{
		MemoryPool	* pool = g_memoryHeap_poolsBySize[i];

		_MESSAGE("%03X: %08X", i << 2, pool);
		gLog.Indent();

		if(pool)
		{
			_MESSAGE("name = %s buffer = %08X size = %08X entry size = %08X page index size = %08X",
				pool->m_name,
				pool->field_040,
				pool->field_110,
				pool->field_100,
				pool->field_10C);

			gLog.Indent();

			for(UInt32 j = 0; j < pool->field_10C; j++)
			{
				_MESSAGE("%08X: %04X", j, pool->field_108[j]);
			}

			gLog.Outdent();
		}

		gLog.Outdent();
	}
#endif

	return true;
}

CommandInfo	kCommandInfo_DebugMemDump =
{
	"DebugMemDump",
	"",
	0,
	"",
	0,
	0,
	NULL,
	HANDLER(Cmd_DebugMemDump_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};
