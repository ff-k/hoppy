#include "android_mengine.h"

#include "mengine_opengles.c"
#include "hoppy.c"

#define SW_RENDERER 0

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
AndroidInitWindow(ANativeWindow * Window){
	if(ANativeWindow_setBuffersGeometry(Window, 0, 0, 
										WINDOW_FORMAT_RGBX_8888) < 0){
		Error("An error occured while setting window's buffer geometry");
	}
}

static void AndroidClearWindow(ANativeWindow_Buffer * WindowBuffer, v3 ClearColor){
#if SW_RENDERER
	u32 ClearColor = (0xFF << 24) |
					(((u32)ClearColor.b) << 16) |
					(((u32)ClearColor.g) <<  8) |
					(((u32)ClearColor.r) <<  0);
	memset(WindowBuffer->bits, ClearColor, 
		   WindowBuffer->stride * WindowBuffer->height * sizeof(u32));
#else
	glClearColor(ClearColor.r, ClearColor.g, ClearColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
#endif
}

static void
AndroidDrawRect(ANativeWindow_Buffer * WindowBuffer, rect Rect, r32 OffsetX, r32 OffsetY){
	u32 * Row = ((u32 *) WindowBuffer->bits) + (WindowBuffer->stride * (u32) Rect.Position.y);
	
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

static void 
AndroidRender(ANativeWindow * Window, opengles_manager * GLESManager, r32 OffsetX, r32 OffsetY){
	if(!WindowInitialised) return;

#if SW_RENDERER
	ANativeWindow_Buffer WindowBuffer;
	if(AndroidLockWindow(Window, &WindowBuffer)){
		v3 ClearColor = {0.0f, 0.0f, 0.0f, 1.0f};
		AndroidClearWindow(&WindowBuffer, ClearColor);

		rect Rect;
		Rect.Size = (v2){(r32)WindowBuffer.width, (r32)WindowBuffer.height};
		Rect.Position = (v2){0.0f, 0.0f};
		AndroidDrawRect(&WindowBuffer, Rect, OffsetX, OffsetY);
	
		AndroidUnlockWindow(Window);
	}
#else
	if(!GLESManager->IsInitialised) return;
	v3 ClearColor = {sinf(OffsetX), cosf(OffsetY), cosf(OffsetX)+sinf(OffsetY)};
	AndroidClearWindow(0, ClearColor);
	if(eglSwapBuffers(GLESManager->Display, GLESManager->Surface) != EGL_TRUE){
		Debug("SwapBuffers failed!");
	}
#endif
}

static void AndroidOnActivate(android_app * App){
#if SW_RENDERER
	AndroidInitWindow(App->window);
#else
	android_shared_data * Shared = App->userData;
	opengles_manager * GLESManager = &Shared->GLESManager;
	
	OpenGLESInit(App->window, GLESManager);
#endif
	WindowInitialised = true;
}

static void AndroidOnDeactivate(android_app * App){
	WindowInitialised = false;

#if !SW_RENDERER
	android_shared_data * Shared = App->userData;
	opengles_manager * GLESManager = &Shared->GLESManager;
	
	OpenGLESStop(GLESManager);
#endif
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
	r32 OffsetX = 0;
	r32 OffsetY = 0;
	while (Shared.IsRunning) {
		/* Process platfrom messages */
		/* Process input */
		/* Game Update */
		/* Audio Update */
		/* Wait for FPS */
		/* Render Frame */


		while (1) {
			Result = ALooper_pollAll(Shared.IsEnabled-1, 0, &Events, (void**) &Source);
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
			AndroidRender(App->window, &Shared.GLESManager, OffsetX, OffsetY);
		}
		OffsetX += 0.01f;
		OffsetY += 0.02f;
	}
}
