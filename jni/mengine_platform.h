#include "mengine_common.h"

#define PlatformMemoryAlloc(name) void * name(memsz size)
typedef PlatformMemoryAlloc(platform_memory_alloc);

typedef struct platform_api{
	platform_memory_alloc * AllocateMemory;
} platform_api;

typedef struct game_memory{
	void * Memory;
	memsz Used;
	memsz Capacity;

	platform_api PlatformAPI;
} game_memory;

#define MEngineUpdate(name) void name(game_memory *Memory)
typedef MEngineUpdate(game_update);
