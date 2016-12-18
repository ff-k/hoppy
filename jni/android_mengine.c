#include "android_mengine.h"

#include "mengine_rng.c"
#include "mengine_asset.c"
#include "mengine_component.c"
#include "mengine_entity.c"
#include "mengine_input.c"
#include "mengine_opengles.c"
#include "mengine_render.c"
#include "hoppy.c"

static void 
SegmentationFaultHandler(int Signal){
	PrintStackTrace;
	exit(0);
}

static PlatformMemoryAlloc(AndroidMemoryAllocate){
	BeginStackTraceBlock;
	void * Allocated = malloc(size);
	if(!Allocated){
		Error("Memory allocation failed!");
	}
	EndStackTraceBlock;
	return Allocated;
}

static void AndroidInitPlatformAPI(platform_api * PlatformAPI){
	BeginStackTraceBlock;
	ZeroStruct(*PlatformAPI);
	PlatformAPI->AllocateMemory = AndroidMemoryAllocate;

	Verbose("Initialised PlatformAPI");
	EndStackTraceBlock;
}

static void 
AndroidLoadGameFunctions(game_functions * Functions){
	BeginStackTraceBlock;
	ZeroStruct(*Functions);

	/* TODO(furkan) : Load from library */
	Functions->Init = GameInit;
	Functions->Update = GameUpdate;

	Verbose("Loaded game functions");
	EndStackTraceBlock;
}

static void 
AndroidInitGameMemory(game_memory * Memory, 
						opengles_manager * GLESManager,
						asset_manager * AssetManager){
	BeginStackTraceBlock;
	ZeroStruct(*Memory);

	Memory->GLESManager = GLESManager;
	Memory->AssetManager = AssetManager;

	Memory->EntitySentinel.Next = &Memory->EntitySentinel;
	Memory->EntitySentinel.Prev = &Memory->EntitySentinel;

	Verbose("Initialised game memory");
	EndStackTraceBlock;
}

static b8 
AndroidLockWindow(ANativeWindow * Window, 
				  ANativeWindow_Buffer * WindowBuffer){
	BeginStackTraceBlock;
	b8 Success = true;
	if(ANativeWindow_lock(Window, WindowBuffer, 0) < 0){
		Error("LockWindow failed!");
		Success = false;
	}
	
	EndStackTraceBlock;
	return Success;
}

static void AndroidUnlockWindow(ANativeWindow * Window){
	BeginStackTraceBlock;
	ANativeWindow_unlockAndPost(Window);
	EndStackTraceBlock;
}

static void
AndroidInitRenderer(android_app * App){
	BeginStackTraceBlock;
	ANativeWindow * Window = App->window;
	android_shared_data * Shared = App->userData;
#if SW_RENDERER
	if(ANativeWindow_setBuffersGeometry(Window, 0, 0, 
										WINDOW_FORMAT_RGBX_8888) < 0){
		Error("An error occured while setting window's buffer geometry");
	}
#else
	opengles_manager * GLESManager = &Shared->GLESManager;
	OpenGLESInit(Window, GLESManager, (v2i *)&Shared->ScreenDim);
#endif
	Shared->RendererAvailable = true;
	EndStackTraceBlock;
}

static void
AndroidDestroyRenderer(android_app * App){
	BeginStackTraceBlock;
	android_shared_data * Shared = App->userData;
#if SW_RENDERER
#else
	opengles_manager * GLESManager = &Shared->GLESManager;
	
	OpenGLESStop(GLESManager);
#endif
	Shared->RendererAvailable = false;
	EndStackTraceBlock;
}


static void 
AndroidRenderFrame(android_app * App, render_commands * Commands){
	BeginStackTraceBlock;
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
		opengles_manager * GLESManager = &Shared->GLESManager;
		OpenGLESRenderCommands(GLESManager, Commands);
//		Shared->IsEnabled = false;
	#endif
	}
	EndStackTraceBlock;
}

static void AndroidOnActivate(android_app * App){
	BeginStackTraceBlock;
	android_shared_data * Shared = App->userData;
	AndroidInitRenderer(App);
	Shared->IsEnabled = true;
	EndStackTraceBlock;
}

static void AndroidOnDeactivate(android_app * App){
	BeginStackTraceBlock;
	android_shared_data * Shared = App->userData;
	AndroidDestroyRenderer(App);
	Shared->IsEnabled = false;
	EndStackTraceBlock;
}

static void 
AndroidChangeInputHandler(android_input_handler * InputHandler, 
								AInputQueue * NewInputQueue){
  	BeginStackTraceBlock;
	if(InputHandler->InputQueue){
   		AInputQueue_detachLooper(InputHandler->InputQueue);
   	}

   	InputHandler->InputQueue = NewInputQueue;
   	if(InputHandler->InputQueue){
   		ALooper * Looper = 
					ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
   		if(Looper){
   			Verbose("Looper is prepared");
			
			if(!InputHandler->InputPipeFDs[0] &&
				!InputHandler->InputPipeFDs[1]){
				if(!pipe(InputHandler->InputPipeFDs)){
			   		/* NOTE(furkan) : 
			   		[0] is the read end, [1] is the write end of the pipe 
			   		*/
					Verbose("Pipe file descriptors are initialised");
				} else {
					Error("pipe() is failed! (%s)", strerror(errno));
	   				/* NOTE(furkan) : Not sure if pipe() has modified them.
	   					Zeroed the fds to make them reliable.
	   				*/
	   				InputHandler->InputPipeFDs[0] = 0;
   					InputHandler->InputPipeFDs[1] = 0;
				}
			} else if(!InputHandler->InputPipeFDs[0] || 
						!InputHandler->InputPipeFDs[1]){
				/* TODO(furkan) : This branch is ought to be deleted 
					in production build. It is written just to catch
					if InputPipeFDs are getting corrupted somewhere.
				*/
				Error("InputPipeFDs are in a corrupted state!");
			}

   			if(InputHandler->InputPipeFDs[0] &&
				InputHandler->InputPipeFDs[1]){
   				/* NOTE(furkan) : addFd returns 1 on success, 
												-1 on fail */
				#define LoopIdentInput 74
				/* TODO(furkan) : Not sure if LoopIdentInput 
									is necessary
				*/
   				s32 Success = ALooper_addFd(Looper, 
							InputHandler->InputPipeFDs[0], 
							LoopIdentInput, ALOOPER_EVENT_INPUT, 0, 0);
				if(Success == 1){
   					Verbose("File descriptor is attached to looper");
   					AInputQueue_attachLooper(InputHandler->InputQueue, 
										Looper, LoopIdentInput, 0, 0);
   					Verbose("InputQueue is attached to looper");
   				} else {
   					Error("Looper_addFd is failed!");
   				}
				#undef LoopIdentInput
   			} else {
   			}
   		} else {
   			Error("Looper_prepare has failed!");
   		}
   	}
	EndStackTraceBlock;
}

void AndroidMainCallback(android_app * App, s32 cmd){
	BeginStackTraceBlock;
	switch(cmd){
    	case APP_CMD_INPUT_CHANGED:{
			Verbose("APP_CMD_INPUT_CHANGED");
			android_shared_data * Shared = App->userData;
			AndroidChangeInputHandler(&Shared->InputHandler, 
										App->inputQueue);
		} break;
    	case APP_CMD_INIT_WINDOW:{
			android_shared_data * Shared = App->userData;
			AndroidOnActivate(App);	

			Shared->ScreenDim = V2U(ANativeWindow_getWidth(App->window),
									ANativeWindow_getHeight(App->window));

			Verbose("APP_CMD_INIT_WINDOW");
		} break;
    	case APP_CMD_TERM_WINDOW:{
			android_shared_data * Shared = App->userData;
			AndroidOnDeactivate(App);

			Shared->ScreenDim = V2U(0, 0);

 			Verbose("APP_CMD_TERM_WINDOW");
		} break;
	   	case APP_CMD_WINDOW_RESIZED:{
			android_shared_data * Shared = App->userData;

			Shared->ScreenDim = V2U(ANativeWindow_getWidth(App->window),
									ANativeWindow_getHeight(App->window));

 			Verbose("APP_CMD_WINDOW_RESIZED");
		} break;
	   	case APP_CMD_WINDOW_REDRAW_NEEDED:
 			Verbose("APP_CMD_WINDOW_REDRAW_NEEDED");
			break;
	   	case APP_CMD_CONTENT_RECT_CHANGED:
 			Verbose("APP_CMD_CONTENT_RECT_CHANGED");
			break;
	   	case APP_CMD_GAINED_FOCUS:
 			Verbose("APP_CMD_GAINED_FOCUS");
			break;
	   	case APP_CMD_LOST_FOCUS:
 			Verbose("APP_CMD_LOST_FOCUS");
			break;
	   	case APP_CMD_CONFIG_CHANGED:
 			Debug("APP_CMD_CONFIG_CHANGED");
			break;
	   	case APP_CMD_LOW_MEMORY:
 			Verbose("APP_CMD_LOW_MEMORY");
			break;
	   	case APP_CMD_START:
 			Verbose("APP_CMD_START");
			break;
	   	case APP_CMD_RESUME:
 			Verbose("APP_CMD_RESUME");
			break;
	   	case APP_CMD_SAVE_STATE:
			Verbose("APP_CMD_SAVE_STATE");
			break;
		case APP_CMD_PAUSE:
   			Verbose("APP_CMD_PAUSE");
			break;
		case APP_CMD_STOP:
 			Verbose("APP_CMD_STOP");
			break;
   		case APP_CMD_DESTROY:
			Verbose("APP_CMD_DESTROY");
			break;
		InvalidDefaultCase;
	}
	EndStackTraceBlock;
}

static void
AndroidHandleInput(android_shared_data * Shared, game_input * GameInput){
	BeginStackTraceBlock;
	Verbose("Handling Input");
	AInputEvent * InputEvent = 0;
	AInputQueue * InputQueue = Shared->InputHandler.InputQueue;
	
	GameInput->PointerCount = 0;
	
	while (InputQueue && 
			AInputQueue_getEvent(InputQueue, &InputEvent) >= 0) {
		if (AInputQueue_preDispatchEvent(InputQueue, InputEvent)) {
	        continue;
	    }
		s32 Handled = 0;
		switch(AInputEvent_getType(InputEvent)){
			case AINPUT_EVENT_TYPE_KEY: {
				Verbose("Key events are not handled");

				s32 Action = AKeyEvent_getAction(InputEvent);
				s32 KeyCode = AKeyEvent_getKeyCode(InputEvent);
				s32 ScanCode = AKeyEvent_getScanCode(InputEvent);
				s32 MetaState = AKeyEvent_getMetaState(InputEvent);
				s32 RepeatCount = AKeyEvent_getRepeatCount(InputEvent);
				switch(Action){
					case AKEY_EVENT_ACTION_DOWN:{
						Verbose("KeyAction_Down");
					} break;
				    case AKEY_EVENT_ACTION_UP:{
						/*switch(KeyCode){
							case AKEYCODE_BACK:{
								Handled = 1;
								Shared->IsEnabled = false;
								Shared->IsRunning = false;
							} break;
						}
						*/
						Verbose("KeyAction_Up");
					} break;
					case AKEY_EVENT_ACTION_MULTIPLE:{
						Verbose("KeyAction_Multiple");
					} break;
					InvalidDefaultCase;
				}
				
				s32 Flags = AKeyEvent_getFlags(InputEvent);
				if(Flags & AKEY_EVENT_FLAG_WOKE_HERE){
					Verbose("KeyEventFlag_WokeHere");
				} 
				if(Flags & AKEY_EVENT_FLAG_SOFT_KEYBOARD){
					Verbose("KeyEventFlag_SoftKeyboard");
				} 
				if(Flags & AKEY_EVENT_FLAG_KEEP_TOUCH_MODE){
					Verbose("KeyEventFlag_KeepTouchMode");
				} 
				if(Flags & AKEY_EVENT_FLAG_FROM_SYSTEM){
					Verbose("KeyEventFlag_FromSystem");
				}
				if(Flags & AKEY_EVENT_FLAG_EDITOR_ACTION){
					Verbose("KeyEventFlag_EditorAction");					
				} 
				if(Flags & AKEY_EVENT_FLAG_CANCELED){
					Verbose("KeyEventFlag_Canceled");					
				}
				if(Flags & AKEY_EVENT_FLAG_VIRTUAL_HARD_KEY){
					Verbose("KeyEventFlag_VirtualHardKey");					
				}
				if(Flags & AKEY_EVENT_FLAG_LONG_PRESS){
					Verbose("KeyEventFlag_LongPress");					
				}
				if(Flags & AKEY_EVENT_FLAG_CANCELED_LONG_PRESS){
					Verbose("KeyEventFlag_CanceledLongPress");					
				}
			    if(Flags & AKEY_EVENT_FLAG_TRACKING){
					Verbose("KeyEventFlag_Tracking");					
				}
    			if(Flags & AKEY_EVENT_FLAG_FALLBACK){
					Verbose("KeyEventFlag_Fallback");					
				}
    				
				Verbose("KeyCode %d, ScanCode : %d, MetaState : %d, RepeatCount : %d", KeyCode, ScanCode, MetaState, RepeatCount);
			} break;
			case AINPUT_EVENT_TYPE_MOTION: {
				switch(AInputEvent_getSource(InputEvent)){
					case AINPUT_SOURCE_UNKNOWN:{
						Verbose("Motion event's source is not handled : Unkown");
					} break; 
				    case AINPUT_SOURCE_KEYBOARD:{
						Verbose("Motion event's source is not handled : Keyboard");
					} break; 
				    case AINPUT_SOURCE_DPAD    :{
						Verbose("Motion event's source is not handled : DPad");
					} break; 
					case AINPUT_SOURCE_GAMEPAD:{
						Verbose("Motion event's source is not handled : Gamepad");
					} break; 
				    case AINPUT_SOURCE_TOUCHSCREEN:{
						u8 Action = AMotionEvent_getAction(InputEvent) & AMOTION_EVENT_ACTION_MASK;
						u8 PointerIndex = (AMotionEvent_getAction(InputEvent) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> 8;
						/* TODO(furkan) : Actually, it is  gotta be 
							checked if PointerIndex is greater than what 
							the engine expect and handle it properly.
							During development, assertion is OK
						*/
						Assert(PointerIndex < MaxPointerCount);
						GameInput->PointerCount = AMotionEvent_getPointerCount(InputEvent);
						GameInput->PointerCoordinates[PointerIndex] = V2(AMotionEvent_getX(InputEvent, PointerIndex), AMotionEvent_getY(InputEvent, PointerIndex));
						Handled = 1;
						switch(Action){
							case AMOTION_EVENT_ACTION_DOWN:{
								Verbose("MotionEvent_Down by Pointer %hhu", PointerIndex);
								GameInput->PointerAction[PointerIndex] = PointerAction_Down;
								GameInput->PointerPressed[PointerIndex] = true;
							} break;
						    case AMOTION_EVENT_ACTION_UP:{
								Verbose("MotionEvent_Up by Pointer %hhu", PointerIndex);
								GameInput->PointerAction[PointerIndex] = PointerAction_Up;
								GameInput->PointerPressed[PointerIndex] = false;
							} break;
						    case AMOTION_EVENT_ACTION_MOVE:{
								Verbose("MotionEvent_Move by Pointer %hhu", PointerIndex);
								GameInput->PointerAction[PointerIndex] = PointerAction_Move;
							} break;
						    case AMOTION_EVENT_ACTION_CANCEL:{
								Verbose("MotionEvent_Cancel by Pointer %hhu", PointerIndex);
							} break;
						    case AMOTION_EVENT_ACTION_OUTSIDE:{
								Verbose("MotionEvent_Outside by Pointer %hhu", PointerIndex);
							} break;
						    case AMOTION_EVENT_ACTION_POINTER_DOWN:{
								Verbose("MotionEvent_PointerDown by Pointer %hhu", PointerIndex);
							} break;
						    case AMOTION_EVENT_ACTION_POINTER_UP:{
								Verbose("MotionEvent_PointerUp by Pointer %hhu", PointerIndex);
							} break;
						    case AMOTION_EVENT_ACTION_HOVER_MOVE:{
								Verbose("MotionEvent_HoverMove by Pointer %hhu", PointerIndex);
							} break;
						    case AMOTION_EVENT_ACTION_SCROLL:{
								Verbose("MotionEvent_Scroll by Pointer %hhu", PointerIndex);
							} break;
						    case AMOTION_EVENT_ACTION_HOVER_ENTER:{
								Verbose("MotionEvent_HoverEnter by Pointer %hhu", PointerIndex);
							} break;
						    case AMOTION_EVENT_ACTION_HOVER_EXIT:{
								Verbose("MotionEvent_HoverExit by Pointer %hhu", PointerIndex);
							} break;
							InvalidDefaultCase;
						}
					} break; 
				    case AINPUT_SOURCE_MOUSE:{
						Verbose("Motion event's source is not handled : Mouse");
					} break; 
				    case AINPUT_SOURCE_STYLUS:{
						Verbose("Motion event's source is not handled : Stylus");
					} break; 
				    case AINPUT_SOURCE_TRACKBALL:{
						Verbose("Motion event's source is not handled : Trackball");
					} break; 
				    case AINPUT_SOURCE_TOUCHPAD:{
						Verbose("Motion event's source is not handled : Touchpad");
					} break; 
				    case AINPUT_SOURCE_TOUCH_NAVIGATION:{
						Verbose("Motion event's source is not handled : Touch navigation");
					} break; 
				    case AINPUT_SOURCE_JOYSTICK:{
						Verbose("Motion event's source is not handled : Joystick");
					} break; 
				    case AINPUT_SOURCE_ANY:{
						Verbose("Motion event's source is not handled : Any");
					} break; 
					InvalidDefaultCase;
				}
			} break;
			InvalidDefaultCase;
		}
    	AInputQueue_finishEvent(InputQueue, InputEvent, Handled);
	}
	EndStackTraceBlock;
}

static void
AndroidInitAssetManager(AAssetManager * AndroidAssetManager,
						asset_manager * AssetManager, 
						char * AssetFileName){
	AAsset * Asset = AAssetManager_open(AndroidAssetManager,
									AssetFileName, AASSET_MODE_STREAMING);
	if(Asset){
		asset_header Header;
		AAsset_read(Asset, &Header.FileFormat, sizeof(Header.FileFormat));
		AAsset_read(Asset, &Header.Version, sizeof(Header.Version));

		Debug("Asset FileFormat : %d,  Version %d", 
										Header.FileFormat,
										Header.Version);

		AAsset_read(Asset, &AssetManager->AssetTypeCount, sizeof(uint16_t));
		Debug("AssetTypeCount : %d", AssetManager->AssetTypeCount);
		AssetManager->AssetTypes = Platform.AllocateMemory(sizeof(asset_type) * AssetManager->AssetTypeCount);
		uint16_t AssetTypeIndex;
		for(AssetTypeIndex = 0; 
			AssetTypeIndex < AssetManager->AssetTypeCount; 
			AssetTypeIndex++){
			
			asset_type * AssetType = AssetManager->AssetTypes + AssetTypeIndex;
			AAsset_read(Asset, &AssetType->ID,sizeof(AssetType->ID));
			AAsset_read(Asset, &AssetType->AssetCount, 
						sizeof(AssetType->AssetCount));
			Debug("AssetTypeID  : %s, AssetCount : %d", AssetType->ID, AssetType->AssetCount);
			
			AssetType->AssetTable = Platform.AllocateMemory(sizeof(asset_table_entry) * AssetType->AssetCount);
			uint32_t EntryIndex;
			for(EntryIndex=0; 
				EntryIndex<AssetType->AssetCount; 
				EntryIndex++){

				asset_table_entry * Entry = AssetType->AssetTable + EntryIndex;
				AAsset_read(Asset, &Entry->ID, sizeof(Entry->ID));
				Debug("Asset ID : %s", Entry->ID);
				AAsset_read(Asset, &Entry->Offset, sizeof(Entry->Offset));
			}
		}

		uint8_t BytesPerPixel = 4;
		asset_type * AssetTypeBitmap = AssetManager->AssetTypes + 0;
		uint32_t BitmapCount = AssetTypeBitmap->AssetCount;
		AssetManager->Bitmaps = Platform.AllocateMemory(sizeof(asset_bitmap) * BitmapCount);
		uint32_t BitmapIndex;
		for(BitmapIndex=0; BitmapIndex<BitmapCount; BitmapIndex++){
			asset_bitmap * Bitmap = AssetManager->Bitmaps + BitmapIndex;

			AAsset_read(Asset, &Bitmap->Width, sizeof(Bitmap->Width));
			AAsset_read(Asset, &Bitmap->Height, sizeof(Bitmap->Height));
			Debug("BitmapIndex :%d, Width : %d, Height : %d", BitmapIndex, Bitmap->Width, Bitmap->Height);

			uint32_t PixelCount = Bitmap->Width * Bitmap->Height;
			Bitmap->Pixels = Platform.AllocateMemory(BytesPerPixel * PixelCount);
			AAsset_read(Asset, Bitmap->Pixels, BytesPerPixel*PixelCount);
		}

		AAsset_close(Asset);
	} else {
		Error("Asset could not be opened!");
	}

}

void android_main(android_app * App) {
	BeginStackTraceBlock;
	Verbose("Entered android_main");
	/* NOTE(furkan) : If you look into android_native_glue.c,
	it seems like app_dummy() function does nothing useful.
	But if you do not call it here, the application will fail at the
	beginning. */
	app_dummy();

	/* Segmentation fault debugger */
#if MENGINE_DEBUG 
	signal(SIGSEGV, SegmentationFaultHandler);
#endif
	/* Initialise android platform layer */
	android_shared_data Shared;
	ZeroStruct(Shared);
	
	App->userData = &Shared;
	App->onAppCmd = AndroidMainCallback;
	
	AndroidInitPlatformAPI(&Platform);

	/*
	Verbose("Internal dir : %s", App->activity->internalDataPath);
	Verbose("External dir : %s", App->activity->externalDataPath);
	*/

	/* Initialise MEngine Functions */
	game_functions Game;
	AndroidLoadGameFunctions(&Game);

	game_input GameInput;
	ZeroStruct(GameInput);
	
	/* Initialise Asset Manager */
	asset_manager AssetManager;
	ZeroStruct(AssetManager);
	AndroidInitAssetManager(App->activity->assetManager,
						&AssetManager, 
						"hoppy_assets.mass");

	/* Initialise Game Memory */
	game_memory GameMemory;
	AndroidInitGameMemory(&GameMemory, &Shared.GLESManager, &AssetManager);

	/* Initialise Render Commands */
	render_commands RenderCommands;
	ZeroStruct(RenderCommands);

	RenderCommands.Entries = AndroidMemoryAllocate(InitialRenderCommandsSize);
	RenderCommands.CapacityInBytes = InitialRenderCommandsSize;

	
	/* Initialise Timer */
	timer Timer;
	Timer.LastCounter = GetCurrentTime();
	Timer.Frequency = GetProcessorFrequency();
	r32 TimeAccumulator = 0.0f;
	/* TODO(furkan) : Detect screen refresh rate and calculate 
		GameFrameMiliseconds using that */
	r32 GameFrameMiliseconds = 1.0f/60.0f;
	Verbose("GameFrameMiliseconds = %f", GameFrameMiliseconds);

	Verbose("Engine is initialised");

	/* TODO(furkan) : Get screen resoulution from Java side */

	Game.Init(&GameMemory);
	
	/* Main game loop */
	s32 Ident;
	s32 Events;
	android_poll_source * Source;

	Shared.IsRunning = true;
	while (Shared.IsRunning) {
		/* Process platfrom messages */
		/* Process input */
		/* Game Update */
		/* Audio Update */
		/* Wait for FPS */
		/* Render Frame */

		RenderCommands.EntryAt = 0;
		GameInput.PointerCount = GameInput.PointerPressed[0] ? 1 : 0;
		GameMemory.ScreenDim = Shared.ScreenDim;
	
		while (Shared.IsRunning) {
			Ident = ALooper_pollAll(Shared.IsEnabled-1, 0, &Events, (void**) &Source);	
	if(Ident < 0){
				break;
			}

			AndroidHandleInput(&Shared, &GameInput);

			if (Source) {
				Source->process(App, Source);
			}
			
			if (App->destroyRequested) {
				Shared.IsRunning = false;
				Shared.IsEnabled = false;
				break;
			}
		}

		u64 CurrentTime = GetCurrentTime();
		r32 TimeDifference = (r32)(((r64)(CurrentTime-Timer.LastCounter))/((r64)Timer.Frequency));
		TimeAccumulator += TimeDifference;
		if(Shared.IsEnabled && Shared.RendererAvailable	){			
			s32 i=0;
			Debug("Frame time %f miliseconds", TimeDifference);

//			while(TimeAccumulator >= GameFrameMiliseconds){
//				Debug("%d. Accumulator %f", i, TimeAccumulator);
				GameInput.DeltaTime = GameFrameMiliseconds;
				Game.Update(&GameMemory, &GameInput);
//				TimeAccumulator -= GameFrameMiliseconds;
//				i++;
//			}
			
			ExtractRenderCommands(&RenderCommands,
									&GameMemory.EntitySentinel, 
									GameMemory.AssetManager);
			
			AndroidRenderFrame(App, &RenderCommands);
		}
		Timer.LastCounter = CurrentTime;
	}
	Verbose("Finished");
	ANativeActivity_finish(App->activity);
	EndStackTraceBlock;
}
