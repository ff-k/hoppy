#ifndef INCLUDED_MENGINE_OPENGLES
#define INCLUDED_MENGINE_OPENGLES

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#define GL_CHECKER \
	if(glGetError() != GL_NO_ERROR){\
		Error("OpenGL ES failed!");\
	}

#define GLESTextureCapacity 64
#define GLESShaderCapacity 16

typedef struct opengles_bitmap_shader{
	GLuint Program;

	GLint ProjectionLocation;
	GLint TextureLocation;
	GLint PositionLocation;
	GLint UVLocation;
} opengles_bitmap_shader;

typedef struct opengles_polygon_shader{
	GLuint Program;

	GLint ProjectionLocation;
	GLint PositionLocation;
	GLint ColorLocation;
} opengles_polygon_shader;

typedef struct opengles_manager{
	EGLDisplay Display;
	EGLSurface Surface;
	EGLContext Context;

	v2u ScreenDim;

	/* TODO (furkan) : Delete them before deactivating the activity */
	GLfloat ProjectionMatrix[4][4];
	GLushort RectIndices[6];
	opengles_bitmap_shader BitmapShader;
	opengles_polygon_shader PolygonShader;
	b8 IsInitialised;
} opengles_manager;

#endif
