#include "android_mengine.h"

typedef struct android_app android_app;
typedef struct android_poll_source android_poll_source;

typedef struct shared_data{
	b8 IsRunning;
	b8 IsEnabled;
} shared_data;

typedef struct rect{
	u32 Width;
	u32 Height;
	u32 Color;

	/* TODO(furkan) : Change them with V2(x,y) */
	/* NOTE(furkan) : That vector points to bottom-left of the rectangle */
	s32 X;
	s32 Y;
} rect;

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

static void
AndroidInitWindow(ANativeWindow * Window){
	if(ANativeWindow_setBuffersGeometry(Window, 0, 0, 
										WINDOW_FORMAT_RGBX_8888) < 0){
		Error("An error occured while setting window's buffer geometry");
	}
}

static void AndroidClearWindow(ANativeWindow * Window){
	u32 ClearColor = 0x0;
	ANativeWindow_Buffer WindowBuffer;
	if(AndroidLockWindow(Window, &WindowBuffer)){
		memset(WindowBuffer.bits, ClearColor, 
			   WindowBuffer.stride * WindowBuffer.height * sizeof(u32));
		AndroidUnlockWindow(Window);
	}
}

static void
AndroidDrawRect(ANativeWindow_Buffer * WindowBuffer, rect Rect){
	u32 * Row = ((u32 *) WindowBuffer->bits) + (WindowBuffer->stride * Rect.Y);
	
	s32 RowIndex;
	s32 ColIndex;
	for(RowIndex=0; RowIndex<Rect.Height; RowIndex++){
		s32 ColumnRightMost = Rect.X + Rect.Width;
		for(ColIndex=Rect.X; ColIndex<ColumnRightMost; ColIndex++){
			Debug("Column : %d %d", RowIndex, ColIndex);
			u32 Red = (u32)((((r32)(ColumnRightMost-ColIndex))/((r32)Rect.Width))*0xFF);
			u32 Green = (u32)((((r32)(Rect.Height-RowIndex))/((r32)Rect.Height))*0xFF);
			u32 Blue = 0;
			Debug("%u %u %u", Red, Green, Blue);
			Row[ColIndex] = (Blue << 16) | (Green << 8) | Red;
		}
		Row += WindowBuffer->stride;
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
			AndroidClearWindow(App->window);

			ANativeWindow_Buffer WindowBuffer;
			AndroidLockWindow(App->window, &WindowBuffer);

			rect Rect;
			Rect.Width = WindowBuffer.width-64;
			Rect.Height = WindowBuffer.height-64;
			Rect.Color = 0x0000FF00;
			Rect.X = 32;
			Rect.Y = 32;
			AndroidDrawRect(&WindowBuffer, Rect);

			AndroidUnlockWindow(App->window);
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
