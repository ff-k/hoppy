#include "android_mengine.h"

#include "mengine_opengles.c"
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

	Debug("Initialised PlatformAPI");
}

static void 
AndroidLoadGameFunctions(android_game_functions * Functions){
	ZeroStruct(*Functions);

	/* TODO(furkan) : Load from library */
	Functions->Update = GameUpdate;

	Debug("Loaded game functions");
}

static void AndroidInitGameMemory(game_memory * Memory){
	ZeroStruct(*Memory);

	Memory->Memory = AndroidMemoryAllocate(InitialGameMemorySize);
	Memory->Used = 0;
	Memory->Capacity = InitialGameMemorySize;

	AndroidInitPlatformAPI(&Memory->PlatformAPI);
	Debug("Initialised game memory");
}

static b8 
AndroidLockWindow(ANativeWindow * Window, 
				  ANativeWindow_Buffer * WindowBuffer){
	b8 Success = true;
	if(ANativeWindow_lock(Window, WindowBuffer, 0) < 0){
		Warning("LockWindow failed!");
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
AndroidInitWindow(ANativeWindow * Window, opengles_manager * GLESManager){
#if 0
	if(ANativeWindow_setBuffersGeometry(Window, 0, 0, 
										WINDOW_FORMAT_RGBX_8888) < 0){
		Error("An error occured while setting window's buffer geometry");
	}
#else
	OpenGLESInit(Window, GLESManager);
#endif
	WindowInitialised = true;
}

static void AndroidClearWindow(ANativeWindow_Buffer * WindowBuffer){
	u32 ClearColor = 0x0;
	memset(WindowBuffer->bits, ClearColor, 
		   WindowBuffer->stride * WindowBuffer->height * sizeof(u32));
}

static void
AndroidDrawRect(ANativeWindow_Buffer * WindowBuffer, rect Rect, r32 OffsetX, r32 OffsetY){
	u32 * Row = ((u32 *) WindowBuffer->bits) + (WindowBuffer->stride * Rect.Position.y);
	
	s32 RowIndex;
	s32 ColIndex;
	for(RowIndex=0; RowIndex<Rect.Size.Height; RowIndex++){
		s32 ColumnRightMost = Rect.Position.x + Rect.Size.Width;
		for(ColIndex=Rect.Position.x; ColIndex<ColumnRightMost; ColIndex++){
			u32 Red = (u32)((((r32)(ColumnRightMost-ColIndex+OffsetX))/((r32)Rect.Size.Width))*0xFF);
			u32 Green = (u32)((((r32)(Rect.Size.Height-RowIndex+OffsetY))/((r32)Rect.Size.Height))*0xFF);
			u32 Blue = 0;
			Row[ColIndex] = (Blue << 16) | (Green << 8) | Red;
		}
		Row += WindowBuffer->stride;
	}
}

static void AndroidRender(ANativeWindow * Window, r32 OffsetX, r32 OffsetY){
	if(!WindowInitialised) return;
	Debug("Rendering");
	ANativeWindow_Buffer WindowBuffer;
	if(AndroidLockWindow(Window, &WindowBuffer)){
		AndroidClearWindow(&WindowBuffer);

		rect Rect;
		Rect.Size = (v2u){WindowBuffer.width, WindowBuffer.height};
		Rect.Position = (v2i){0, 0};
		AndroidDrawRect(&WindowBuffer, Rect, OffsetX, OffsetY);
	
		AndroidUnlockWindow(Window);
	}
}

static void AndroidOnActivate(android_app * App){
	ANativeWindow_Buffer WindowBuffer;
	if(AndroidLockWindow(App->window, &WindowBuffer)){
		android_shared_data * Shared = App->userData;
		opengles_manager * GLESManager = &Shared->GLESManager;
		GLESManager->ScreenDim = (v2u){WindowBuffer.width, WindowBuffer.height};

		AndroidUnlockWindow(App->window);
		AndroidInitWindow(App->window, GLESManager);
	} else {
		Debug("AndroidLockWindow failed in onActivate");
	}
}

static void AndroidOnDeactivate(android_app * App){
	WindowInitialised = false;
	
	android_shared_data * Shared = App->userData;
	opengles_manager * GLESManager = &Shared->GLESManager;
	
	OpenGLESStop(GLESManager);
}

void AndroidMainCallback(android_app * App, s32 cmd){
	switch(cmd){
    	case APP_CMD_INPUT_CHANGED:
			Debug("APP_CMD_INPUT_CHANGED");
			break;
    	case APP_CMD_INIT_WINDOW:
			AndroidOnActivate(App);
			Debug("APP_CMD_INIT_WINDOW");
			break;
    	case APP_CMD_TERM_WINDOW:
			AndroidOnDeactivate(App);
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
	android_shared_data Shared;
	ZeroStruct(Shared);
	
	App->userData = &Shared;
	App->onAppCmd = AndroidMainCallback;

	Shared.IsRunning = true;
	Shared.IsEnabled = true;
	
	s32 Result;
	s32 Events;
	android_poll_source * Source;
	
	Debug("Internal dir : %s", App->activity->internalDataPath);
	Debug("External dir : %s", App->activity->externalDataPath);

	/* NOTE(furkan) : If you look into android_native_glue.c,
	it seems like app_dummy() function does nothing useful.
	But if you do not call it here, the application will fail at the
	beginning. */
	app_dummy();

	/* Initialise MEngine */
	android_game_functions Game;
	AndroidLoadGameFunctions(&Game);

	game_memory GameMemory;
	AndroidInitGameMemory(&GameMemory);
	
	/* Main game loop */
	Debug("Entering main loop");
	r32 OffsetX = 0;
	r32 OffsetY = 0;
	while (Shared.IsRunning) {
		/* Process platfrom messages */
		/* Process input */
		/* Game Update */
		/* Audio Update */
		/* Wait for FPS */
		/* Render Frame */


		Debug("Entering inner loop");
		while (1) {
			Debug("Polling");
			Result = ALooper_pollAll(Shared.IsEnabled-1, 0, &Events, (void**) &Source);
			Debug("Polled");
			if(Result < 0){
				break;
			}

			if (Source) {
				Debug("Processing source");
				Source->process(App, Source);
			} else {
				Debug("Source is null");
			}
			
			if (App->destroyRequested) {
				Debug("Destroy Requested");
				Shared.IsRunning = false;
				Shared.IsEnabled = false;
				break;
			}
		}
		Debug("Exited inner loop");

		if(Shared.IsEnabled){
			Debug("Updating game and rendering");
			Game.Update(&GameMemory);
//			AndroidRender(App->window, OffsetX, OffsetY);
		} else {
			Debug("App is not enabled");
		}
		OffsetX += 4.0f;
		OffsetY += 4.0f;
	}
}
