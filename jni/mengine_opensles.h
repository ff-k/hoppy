#ifndef INCLUDED_MENGINE_OPENSLES
#define INCLUDED_MENGINE_OPENSLES

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

typedef struct opensles_manager {
	SLEngineItf EngineInterface;

	SLObjectItf Engine;
	SLObjectItf OutputMix;

	SLPlayItf Player;
	SLSeekItf Seek;
	SLObjectItf BackgroundMusic;
} opensles_manager;

#endif
