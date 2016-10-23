#ifndef INCLUDED_MENGINE_OPENGLES
#define INCLUDED_MENGINE_OPENGLES

#include <GLES2/gl2.h>
#include <EGL/egl.h>

typedef struct opengles_manager{
	EGLDisplay Display;
	EGLSurface Surface;
	EGLContext Context;

	v2u ScreenDim;
} opengles_manager;

#endif
