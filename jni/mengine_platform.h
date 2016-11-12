#include "mengine_common.h"
#include "mengine_math.h"
#include "mengine_timer.h"
#include "mengine_asset.h"
#include "mengine_render.h"
#include "mengine_opengles.h"

/* Platform API */
#define PlatformMemoryAlloc(name) void * name(memsz size)
typedef PlatformMemoryAlloc(platform_memory_alloc);

typedef struct platform_api{
	platform_memory_alloc * AllocateMemory;
} platform_api;


/* Engine API */
#define MaxPointerCount 10
typedef enum {
	PointerAction_None,

	PointerAction_Down,
	PointerAction_Up,
	PointerAction_Move,

	PointerAction_Count
} pointer_action;

typedef struct game_input{
	v2 PointerCoordinates[MaxPointerCount];
	pointer_action PointerAction[MaxPointerCount];
	b8 PointerPressed[MaxPointerCount];
	u32 PointerCount;

	r32 DeltaTime;
} game_input;

typedef struct game_memory{
	void * Memory;
	memsz Used;
	memsz Capacity;
	
	v2 Screen;

	opengles_manager * GLESManager;
	asset_manager * AssetManager;

	platform_api PlatformAPI;
} game_memory;

#define MEngineInit(name) void name(game_memory * Memory)
typedef MEngineInit(game_init);

#define MEngineUpdate(name) void name(game_memory * Memory, render_commands * RenderCommands, game_input * Input)
typedef MEngineUpdate(game_update);

typedef struct game_functions{
	game_init * Init;
	game_update * Update;
} game_functions;
