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
    	case APP_CMD_INIT_WINDOW:
    	case APP_CMD_TERM_WINDOW:
    	case APP_CMD_WINDOW_RESIZED:
    	case APP_CMD_WINDOW_REDRAW_NEEDED:
    	case APP_CMD_CONTENT_RECT_CHANGED:
    	case APP_CMD_GAINED_FOCUS:
    	case APP_CMD_LOST_FOCUS:
    	case APP_CMD_CONFIG_CHANGED:
    	case APP_CMD_LOW_MEMORY:
    	case APP_CMD_START:
    	case APP_CMD_RESUME:
    	case APP_CMD_SAVE_STATE:
    	case APP_CMD_PAUSE:
    	case APP_CMD_STOP:
    	case APP_CMD_DESTROY:
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
