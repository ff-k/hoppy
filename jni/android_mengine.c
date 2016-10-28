#include "android_mengine.h"

#include "mengine_opengles.c"
#include "mengine_render.c"
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
AndroidLoadGameFunctions(game_functions * Functions){
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

static void
AndroidInitRenderer(android_app * App){
	ANativeWindow * Window = App->window;
	android_shared_data * Shared = App->userData;
#if SW_RENDERER
	if(ANativeWindow_setBuffersGeometry(Window, 0, 0, 
										WINDOW_FORMAT_RGBX_8888) < 0){
		Error("An error occured while setting window's buffer geometry");
	}
#else
	opengles_manager * GLESManager = &Shared->GLESManager;
	OpenGLESInit(Window, GLESManager);
#endif
	Shared->RendererAvailable = true;
}

static void 
AndroidRenderFrame(android_app * App, render_commands * Commands){
	android_shared_data * Shared = App->userData;
	if(Shared->RendererAvailable){
	#if SW_RENDERER
		ANativeWindow_Buffer WindowBuffer;
		if(AndroidLockWindow(App->window, &WindowBuffer)){
			framebuffer Framebuffer;
			Framebuffer.Data = WindowBuffer.bits;
	    	Framebuffer.Width = WindowBuffer.width;
	    	Framebuffer.Height = WindowBuffer.height;
	    	Framebuffer.Stride = WindowBuffer.stride;
	
			SWRenderCommands(&Framebuffer, Commands);
			AndroidUnlockWindow(App->window);
		}
	#else
		OpenGLESRenderCommands(Commands);
	#endif
	}
}

static void AndroidOnActivate(android_app * App){
	AndroidInitRenderer(App);
}

static void AndroidOnDeactivate(android_app * App){
	android_shared_data * Shared = App->userData;
	Shared->RendererAvailable = false;
#if SW_RENDERER
#else
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
	game_functions Game;
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

		render_commands RenderCommands;
		ZeroStruct(RenderCommands);
		if(Shared.IsEnabled){
			Game.Update(&GameMemory);
			AndroidRenderFrame(App, &RenderCommands);
		}
		OffsetX += 0.01f;
		OffsetY += 0.02f;
	}
}
