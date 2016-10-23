#include "mengine_opengles.h"

static void 
OpenGLESInit(ANativeWindow * Window, opengles_manager * Manager){
	EGLint Format, NumConfigs;
	GLenum Status;
	EGLConfig Config;

	const EGLint DISPLAY_ATTRIBS[] = {
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_BLUE_SIZE, 5, 
		EGL_GREEN_SIZE, 6, 
		EGL_RED_SIZE, 5,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		0
	};
	
	const EGLint CONTEXT_ATTRIBS[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		0
	};

	Manager->Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	b8 Success = Manager->Display != EGL_NO_DISPLAY;
	if (Success){
		Success = eglInitialize(Manager->Display, 0, 0);
		if (Success){
			Success = eglChooseConfig(Manager->Display, DISPLAY_ATTRIBS, &Config, 1, &NumConfigs);
			Success &= (NumConfigs > 0);
			if(Success){
				Success = eglGetConfigAttrib(Manager->Display, Config, EGL_NATIVE_VISUAL_ID, &Format);
				if (Success){
					ANativeWindow_setBuffersGeometry(Window, 0, 0, Format);
					Manager->Surface = eglCreateWindowSurface(Manager->Display, Config, Window, 0);
					Success = Manager->Surface != EGL_NO_SURFACE;
					if (Success){
						Manager->Context = eglCreateContext(Manager->Display, Config, NULL, CONTEXT_ATTRIBS);
						Success = Manager->Context != EGL_NO_CONTEXT;
						if (Success){
							Success = eglMakeCurrent(Manager->Display, Manager->Surface, Manager->Surface, Manager->Context);
							Success &= eglQuerySurface(Manager->Display, Manager->Surface, EGL_WIDTH, &Manager->ScreenDim.Width);
							Success &= eglQuerySurface(Manager->Display, Manager->Surface, EGL_HEIGHT, &Manager->ScreenDim.Height);
							Success &= (Manager->ScreenDim.Width > 0) && (Manager->ScreenDim.Height > 0);
							if (Success){
								glViewport(0, 0, Manager->ScreenDim.Width, Manager->ScreenDim.Height);
								glDisable(GL_DEPTH_TEST);
							}
						}
					}		
				}
			}
		}
	}
}

static void OpenGLESStop(opengles_manager * Manager){
	if (Manager->Display != EGL_NO_DISPLAY) {
		eglMakeCurrent(Manager->Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

		if (Manager->Context != EGL_NO_CONTEXT) {
			eglDestroyContext(Manager->Display, Manager->Context);
			Manager->Context = EGL_NO_CONTEXT;
		}

		if (Manager->Surface != EGL_NO_SURFACE) {
			eglDestroySurface(Manager->Display, Manager->Surface);
			Manager->Surface = EGL_NO_SURFACE;
		}

		eglTerminate(Manager->Display);
		Manager->Display = EGL_NO_DISPLAY;
	}
}

