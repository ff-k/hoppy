#ifndef INCLUDED_MENGINE_PLATFORM
#define INCLUDED_MENGINE_PLATFORM

/* Type Definitions */
typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef size_t memsz;

typedef double r64;
typedef float r32;

typedef u64 b64;
typedef u32 b32;
typedef u16 b16;
typedef u8 b8;

#define true 1
#define false 0

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

#endif
