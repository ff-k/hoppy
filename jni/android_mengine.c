#include "android_mengine.h"

typedef struct android_app android_app;
typedef struct android_poll_source android_poll_source;

typedef struct shared_data{
	b8 IsRunning;
	b8 IsEnabled;
} shared_data;

void AndroidMainCallback(android_app * App, s32 cmd){
	switch(cmd){
		/* TODO Log Events */
    	case APP_CMD_INPUT_CHANGED:
			Debug("APP_CMD_INPUT_CHANGED");
			break;
    	case APP_CMD_INIT_WINDOW:
			Debug("APP_CMD_INIT_WINDOW");
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
	shared_data Shared;
	ZeroStruct(Shared);
	
	App->userData = &Shared;
	App->onAppCmd = AndroidMainCallback;

	Shared.IsRunning = true;
	
	s32 Result;
	s32 Events;
	android_poll_source * Source;


	/*	TODO : Delete app_dummy() call
		Use this function only to make sure that glue works properly 
	*/
	app_dummy();

	while (Shared.IsRunning) {
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
			/* TODO(furkan) : Game loop will be here, I guess */
		}
	}
}
