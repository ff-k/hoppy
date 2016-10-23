#include "android_mengine.h"
#include "mengine_rect.h"

#include "hoppy.c"

static PlatformMemoryAlloc(AndroidMemoryAllocate){
	void * Allocated = malloc(size);
	if(!Allocated){
		Warning("Memory allocation failed!");
	}

	return Allocated;
}

static void AndroidInitPlatformAPI(platform_api * PlatformAPI){
	ZeroStruct(*PlatformAPI);

	PlatformAPI->AllocateMemory = AndroidMemoryAllocate;
}

static void 
AndroidLoadGameFunctions(android_game_functions * Functions){
	ZeroStruct(*Functions);

	/* TODO(furkan) : Load from library */
	Functions->Update = GameUpdate;
}

static void AndroidInitGameMemory(game_memory * Memory){
	ZeroStruct(*Memory);

	Memory->Memory = AndroidMemoryAllocate(InitialGameMemorySize);
	Memory->Used = 0;
	Memory->Capacity = InitialGameMemorySize;

	AndroidInitPlatformAPI(Memory->PlatformAPI);
}

static b8 
AndroidLockWindow(ANativeWindow * Window, 
				  ANativeWindow_Buffer * WindowBuffer){
	b8 Success = true;
	if(ANativeWindow_lock(Window, WindowBuffer, 0) < 0){
		Success = false;
	}
	return Success;
}

static void AndroidUnlockWindow(ANativeWindow * Window){
	ANativeWindow_unlockAndPost(Window);
}

/*	TODO(furkan) : Apparently, having a global variable for this thing
is not good. WindowInitialised will be deleted */
static b8 WindowInitialised = false;
static void
AndroidInitWindow(ANativeWindow * Window){
	if(ANativeWindow_setBuffersGeometry(Window, 0, 0, 
										WINDOW_FORMAT_RGBX_8888) < 0){
		Error("An error occured while setting window's buffer geometry");
	}
	WindowInitialised = true;
}

static void AndroidClearWindow(ANativeWindow_Buffer * WindowBuffer){
	u32 ClearColor = 0x0;
	memset(WindowBuffer->bits, ClearColor, 
		   WindowBuffer->stride * WindowBuffer->height * sizeof(u32));
}

static void
AndroidDrawRect(ANativeWindow_Buffer * WindowBuffer, rect Rect){
	u32 * Row = ((u32 *) WindowBuffer->bits) + (WindowBuffer->stride * Rect.Y);
	
	s32 RowIndex;
	s32 ColIndex;
	for(RowIndex=0; RowIndex<Rect.Height; RowIndex++){
		s32 ColumnRightMost = Rect.X + Rect.Width;
		for(ColIndex=Rect.X; ColIndex<ColumnRightMost; ColIndex++){
			u32 Red = (u32)((((r32)(ColumnRightMost-ColIndex))/((r32)Rect.Width))*0xFF);
			u32 Green = (u32)((((r32)(Rect.Height-RowIndex))/((r32)Rect.Height))*0xFF);
			u32 Blue = 0;
			Row[ColIndex] = (Blue << 16) | (Green << 8) | Red;
		}
		Row += WindowBuffer->stride;
	}
}

static void AndroidRender(ANativeWindow * Window){
	if(!WindowInitialised) return;

	ANativeWindow_Buffer WindowBuffer;
	if(AndroidLockWindow(Window, &WindowBuffer)){
		AndroidClearWindow(&WindowBuffer);

		rect Rect;
		Rect.Width = WindowBuffer.width-64;
		Rect.Height = WindowBuffer.height-64;
		Rect.Color = 0x0000FF00;
		Rect.X = 32;
		Rect.Y = 32;
		AndroidDrawRect(&WindowBuffer, Rect);
	
		AndroidUnlockWindow(Window);
	}
}

void AndroidMainCallback(android_app * App, s32 cmd){
	switch(cmd){
    	case APP_CMD_INPUT_CHANGED:
			Debug("APP_CMD_INPUT_CHANGED");
			break;
    	case APP_CMD_INIT_WINDOW:
			Debug("APP_CMD_INIT_WINDOW");
			AndroidInitWindow(App->window);
			break;
    	case APP_CMD_TERM_WINDOW:
 			Debug("APP_CMD_TERM_WINDOW");
			break;
	   	case APP_CMD_WINDOW_RESIZED:
 			Debug("APP_CMD_WINDOW_RESIZED");
			break;
	   	case APP_CMD_WINDOW_REDRAW_NEEDED:
 			Debug("APP_CMD_WINDOW_REDRAW_NEEDED");
			break;
	   	case APP_CMD_CONTENT_RECT_CHANGED:
 			Debug("APP_CMD_CONTENT_RECT_CHANGED");
			break;
	   	case APP_CMD_GAINED_FOCUS:
 			Debug("APP_CMD_GAINED_FOCUS");
			break;
	   	case APP_CMD_LOST_FOCUS:
 			Debug("APP_CMD_LOST_FOCUS");
			break;
	   	case APP_CMD_CONFIG_CHANGED:
 			Debug("APP_CMD_CONFIG_CHANGED");
			break;
	   	case APP_CMD_LOW_MEMORY:
 			Debug("APP_CMD_LOW_MEMORY");
			break;
	   	case APP_CMD_START:
 			Debug("APP_CMD_START");
			break;
	   	case APP_CMD_RESUME:
 			Debug("APP_CMD_RESUME");
			break;
	   	case APP_CMD_SAVE_STATE:
			Debug("APP_CMD_SAVE_STATE");
			break;
		case APP_CMD_PAUSE:
   			Debug("APP_CMD_PAUSE");
			break;
		case APP_CMD_STOP:
 			Debug("APP_CMD_STOP");
			break;
   		case APP_CMD_DESTROY:
			Debug("APP_CMD_DESTROY");
			break;
		default:
			Warning("Some commands are missing!")
			break;
	}
}

void android_main(android_app * App) {

	/* Initialise android platform layer */
	shared_data Shared;
	ZeroStruct(Shared);
	
	App->userData = &Shared;
	App->onAppCmd = AndroidMainCallback;

	Shared.IsRunning = true;
	
	s32 Result;
	s32 Events;
	android_poll_source * Source;

	/* NOTE(furkan) : If you look into android_native_glue.c,
	it seems like app_dummy() function does nothing useful.
	But if you do not call it here, the application will fail at the
	beginning. */
	app_dummy();

	/* Initialise MEngine */
	android_game_functions Game;
//	AndroidLoadGameFunctions(&Game);

	game_memory GameMemory;
//	AndroidInitGameMemory(&GameMemory);
	
	/* Main game loop */
	while (Shared.IsRunning) {
		/* Process platfrom messages */
		/* Process input */
		/* Game Update */
		/* Audio Update */
		/* Wait for FPS */
		/* Render Frame */


		while (1) {
			Result = ALooper_pollAll(-1, 0, &Events, (void**) &Source);
			if(Result < 0){
				break;
			}

			if (Source) {
				Source->process(App, Source);
			}
			
			if (App->destroyRequested) {
				Shared.IsRunning = false;
				Shared.IsEnabled = false;
				break;
			}
		}

		if(Shared.IsEnabled){
			Game.Update(&GameMemory);
			AndroidRender(App->window);
		}
	}
}
