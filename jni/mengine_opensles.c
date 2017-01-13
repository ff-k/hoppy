static b32
PlayCurrentBackgroundMusic(opensles_manager * Manager){
	b32 Result = true;
	
	if(Manager->Seek && Manager->Player){
		if((*Manager->Seek)->SetLoop(Manager->Seek, SL_BOOLEAN_TRUE, 0, 
								SL_TIME_UNKNOWN) == SL_RESULT_SUCCESS){
			if((*Manager->Player)->SetPlayState(Manager->Player, 
							SL_PLAYSTATE_PLAYING) == SL_RESULT_SUCCESS){
				/* NOTE(furkan) : Success */
			} else {
				Result = false;
			}
		} else {
			Result = false;
		}
	} else {
		Result = false;
	}

	return Result;
}

static void 
StopCurrentBackgroundMusic(opensles_manager * Manager){
	if(Manager->Player && Manager->BackgroundMusic){
		SLuint32 PlayerState;
		(*Manager->BackgroundMusic)->GetState(Manager->BackgroundMusic, 
												&PlayerState);
		if(PlayerState == SL_OBJECT_STATE_REALIZED){
			(*Manager->Player)->SetPlayState(Manager->Player, 
												SL_PLAYSTATE_PAUSED);
			(*Manager->BackgroundMusic)->Destroy(Manager->BackgroundMusic);
			Manager->BackgroundMusic = 0;
			Manager->Player = 0;
			Manager->Seek = 0;
		}
	}
}

static b32
ChangeBackgroundMusic(opensles_manager * Manager, 
						asset_file * BackgroundMusicFile){
	b32 Result = true;

	OpenFileDescriptor(BackgroundMusicFile);

	SLDataLocator_AndroidFD FileDataLocator;
	FileDataLocator.locatorType = SL_DATALOCATOR_ANDROIDFD;
	FileDataLocator.fd = BackgroundMusicFile->Descriptor;
	FileDataLocator.offset = BackgroundMusicFile->Offset;
	FileDataLocator.length = BackgroundMusicFile->Length;

	SLDataFormat_MIME DataFormat;
	DataFormat.formatType = SL_DATAFORMAT_MIME;
	DataFormat.mimeType = 0;
	DataFormat.containerType = SL_CONTAINERTYPE_UNSPECIFIED;

	SLDataSource DataSource;
	DataSource.pLocator = &FileDataLocator;
	DataSource.pFormat = &DataFormat;

	SLDataLocator_OutputMix OutputDataLocator;
	OutputDataLocator.locatorType = SL_DATALOCATOR_OUTPUTMIX;
	OutputDataLocator.outputMix = Manager->OutputMix;

	SLDataSink DataSink;
	DataSink.pLocator = &OutputDataLocator;
	DataSink.pFormat = 0;

	SLuint32 BackgroundPlayerIDCount = 2;
	SLInterfaceID BackgroundPlayerIDs[] = {SL_IID_PLAY, SL_IID_SEEK};
	SLboolean BackgroundPlayerRequired[] = {SL_BOOLEAN_TRUE, 
												SL_BOOLEAN_TRUE};
	SLObjectItf BackgroundMusic;
	SLPlayItf Player;
	SLSeekItf Seek;
	SLEngineItf Engine = Manager->EngineInterface;
	if((*Engine)->CreateAudioPlayer(Engine, &BackgroundMusic, 
						&DataSource, &DataSink, 
						BackgroundPlayerIDCount, BackgroundPlayerIDs, 
						BackgroundPlayerRequired) == SL_RESULT_SUCCESS){
		if((*BackgroundMusic)->Realize(BackgroundMusic, 
							SL_BOOLEAN_FALSE) == SL_RESULT_SUCCESS){
			Manager->BackgroundMusic = BackgroundMusic;
			if((*BackgroundMusic)->GetInterface(BackgroundMusic, 
							SL_IID_PLAY, &Player) == SL_RESULT_SUCCESS){
				Manager->Player = Player;
			} else {
				Result = false;
			}

			if((*BackgroundMusic)->GetInterface(BackgroundMusic, 
							SL_IID_SEEK, &Seek) == SL_RESULT_SUCCESS){
				Manager->Seek = Seek;
			} else {
				Result = false;
			}
		} else {
			Result = false;
		}
	} else {
		Result = false;
	}

	return Result;
}

static b32
InitOpenSLESManager(opensles_manager * Manager, 
					asset_file * BackgroundMusicFile){	
	b32 Result = true;

	ZeroStruct(*Manager);

	SLEngineItf EngineInterface;
	SLObjectItf Engine;
	SLObjectItf OutputMix;

	SLuint32 EngineIDCount = 1;
	SLInterfaceID EngineIDs[] = {SL_IID_ENGINE};
	SLboolean EngineRequired[] = {SL_BOOLEAN_TRUE};

	SLuint32 OutputMixIDCount = 0;
	SLInterfaceID OutputMixIDs[] = {};
	SLboolean OutputMixRequired[] = {};

	/* TODO(furkan) : Free resources on errors */

	if(slCreateEngine(&Engine, 0, 0, EngineIDCount, EngineIDs,
						EngineRequired) == SL_RESULT_SUCCESS){
		if((*Engine)->Realize(Engine, 
						SL_BOOLEAN_FALSE) == SL_RESULT_SUCCESS){
			if((*Engine)->GetInterface(Engine, SL_IID_ENGINE,
						&EngineInterface) == SL_RESULT_SUCCESS){
				/*	NOTE(furkan) : Successfully initialised engine */
			} else {
				Result = false;
			}
		} else {
			Result = false;	
		}
	} else {
		Result = false;
	}

	if(Result){
		if((*EngineInterface)->CreateOutputMix(EngineInterface, 
						&OutputMix, OutputMixIDCount, OutputMixIDs, 
						OutputMixRequired) == SL_RESULT_SUCCESS){
			if((*OutputMix)->Realize(OutputMix, 
						SL_BOOLEAN_FALSE) == SL_RESULT_SUCCESS){
				/*	NOTE(furkan) : Successfully initialised output mix  */
				Manager->EngineInterface = EngineInterface;
				Manager->Engine = Engine;
				Manager->OutputMix = OutputMix;
			} else {
				Result = false;
			}
		} else {
			Result = false;	
		}
	}

	if(BackgroundMusicFile){
		ChangeBackgroundMusic(Manager, BackgroundMusicFile);
	}

	return Result;
}

static void
DestroyOpenSLESManager(opensles_manager * Manager){
	StopCurrentBackgroundMusic(Manager);

	if(Manager->OutputMix){
		(*Manager->OutputMix)->Destroy(Manager->OutputMix);
	}

	if(Manager->Engine){
		(*Manager->Engine)->Destroy(Manager->Engine);
	}
}
