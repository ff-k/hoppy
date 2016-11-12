#include "mengine_opengles.h"

char * TestVertexShader ="\
	attribute vec4 aPosition;\
	attribute vec2 aTexture;\
	\
	varying vec2 vTexture;\
	uniform mat4 uProjection;\
	\
	void main() {\
		vTexture = aTexture;\
		gl_Position = uProjection * aPosition;\
	}";

char * TestFragmentShader ="\
	precision mediump float;\
	\
	varying vec2 vTexture;\
	uniform sampler2D uTexture;\
	\
	void main() {\
		gl_FragColor = texture2D(uTexture, vTexture);\
	}";

static GLuint
OpenGLESLoadTexture(opengles_manager * Manager,
					asset_bitmap * TextureBitmap){
	GLuint Texture;
	GLuint Format = GL_RGBA;

	glGenTextures(1, &Texture);
	GL_CHECKER
	glBindTexture(GL_TEXTURE_2D, Texture);
	GL_CHECKER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	GL_CHECKER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	GL_CHECKER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	GL_CHECKER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GL_CHECKER
	glTexImage2D(GL_TEXTURE_2D, 0, Format, 
					TextureBitmap->Width, TextureBitmap->Height, 0,
					Format, GL_UNSIGNED_BYTE, TextureBitmap->Data);
	GL_CHECKER
	glBindTexture(GL_TEXTURE_2D, 0);
	GL_CHECKER
	if(glGetError() != GL_NO_ERROR){
		Error("An error occured while loading the texture");
		Texture = -1;
	}

	return Texture;
}

static GLuint
OpenGLESLoadShader(opengles_manager * Manager, 
					const char * VertexShaderSource, 
					const char * FragmentShaderSource){
	GLint Result;
	GLuint VertexShader, FragmentShader, ShaderProgram;

	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShader, 1, &VertexShaderSource, 0);
	glCompileShader(VertexShader);
	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Result);
	if (Result == GL_FALSE) {
		Error("Vertex shader compilation failed!");
	}

	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShader, 1, &FragmentShaderSource, 0);
	glCompileShader(FragmentShader);
	glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &Result);
	if (Result == GL_FALSE) {
		Error("Fragment shader compilation failed!");
	}

	ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, VertexShader);
	glAttachShader(ShaderProgram, FragmentShader);
	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Result);

	glDetachShader(ShaderProgram, VertexShader);
	glDeleteShader(VertexShader);
	glDetachShader(ShaderProgram, FragmentShader);
	glDeleteShader(FragmentShader);
	if (Result == GL_FALSE){
		Error("Shader program could not been linked!");
		ShaderProgram = -1;
	}

	return ShaderProgram;
}

static void
OpenGLESInitShaders(opengles_manager * Manager){
	opengles_bitmap_shader * BitmapShader = &Manager->BitmapShader;
	BitmapShader->Program = OpenGLESLoadShader(Manager, TestVertexShader, TestFragmentShader);
	Assert(BitmapShader->Program != -1);

	BitmapShader->PositionLocation = glGetAttribLocation(BitmapShader->Program, "aPosition");
	BitmapShader->UVLocation = glGetAttribLocation(BitmapShader->Program, "aTexture");
	BitmapShader->ProjectionLocation = glGetUniformLocation(BitmapShader->Program,"uProjection");
	BitmapShader->TextureLocation = glGetUniformLocation(BitmapShader->Program, "uTexture");
}

static void 
OpenGLESInit(ANativeWindow * Window, opengles_manager * Manager){
	EGLint Format, NumConfigs;
	GLenum Status;
	EGLConfig Config;

	const EGLint DISPLAY_ATTRIBS[] = {
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_BLUE_SIZE, 8, 
		EGL_GREEN_SIZE, 8, 
		EGL_RED_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};
	
	const EGLint CONTEXT_ATTRIBS[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
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
								
								memset(Manager->ProjectionMatrix[0],
									0, sizeof(Manager->ProjectionMatrix));
								Manager->ProjectionMatrix[0][0] = 2.0f / (GLfloat)Manager->ScreenDim.Width;
								Manager->ProjectionMatrix[1][1] = 2.0f / (GLfloat)Manager->ScreenDim.Height;
								Manager->ProjectionMatrix[2][2] = -1.0f;
								Manager->ProjectionMatrix[3][0] = -1.0f;
								Manager->ProjectionMatrix[3][1] = -1.0f;
								Manager->ProjectionMatrix[3][3] =  1.0f;
				
								Manager->BitmapIndices[0] = 0;
								Manager->BitmapIndices[1] = 1;
								Manager->BitmapIndices[2] = 2;
								Manager->BitmapIndices[3] = 2;
								Manager->BitmapIndices[4] = 3;
								Manager->BitmapIndices[5] = 0;

								OpenGLESInitShaders(Manager);

								Manager->IsInitialised = true;
								Verbose("OpenGL ES initialised!");

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
		Manager->IsInitialised = false;
	}
}

static void 
OpenGLESRenderCommands(opengles_manager * Manager, 
						render_commands * Commands){
	u8 * EntryAt = (u8 *)Commands->Entries;
	u8 * EntryAtLimit = (u8 *)(Commands->Entries) + Commands->EntryAt;
	int Count = 0;
	for(EntryAt = (u8 *)Commands->Entries; 
		EntryAt<EntryAtLimit;){
//		Error("EntryAt : %p, EntryAtLimit : %p", EntryAt, EntryAtLimit);

		render_command_entry * Entry = (render_command_entry *) EntryAt;
		EntryAt += sizeof(render_command_entry);
//		Debug("Processing %dth Entry", ++Count);
		switch(Entry->Type){
			case RenderCommandEntryType_DrawBitmap:{
				render_command_entry_drawbitmap * Command = (render_command_entry_drawbitmap *)EntryAt;
				EntryAt += sizeof(render_command_entry_drawbitmap);
				
				opengles_bitmap_shader * Shader = &Manager->BitmapShader;

				glUseProgram(Shader->Program);
				GL_CHECKER
				glUniformMatrix4fv(Shader->ProjectionLocation, 1, 
					GL_FALSE, (GLfloat *)Manager->ProjectionMatrix);
				GL_CHECKER
				glUniform1i(Shader->TextureLocation, 0);
				GL_CHECKER
				glEnableVertexAttribArray(Shader->PositionLocation);
				GL_CHECKER	
				glVertexAttribPointer(Shader->PositionLocation, 4,
					GL_FLOAT, GL_FALSE, sizeof(v4), Command->Position);
				GL_CHECKER
				glEnableVertexAttribArray(Shader->UVLocation);
				GL_CHECKER	
				glVertexAttribPointer(Shader->UVLocation, 2,
					GL_FLOAT, GL_FALSE, sizeof(v2), Command->UV);
				GL_CHECKER
				glEnable(GL_BLEND);
				GL_CHECKER	
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				GL_CHECKER
				GLuint Texture = OpenGLESLoadTexture(Manager, Command->Bitmap);
				glActiveTexture(GL_TEXTURE0);
				GL_CHECKER	
				glBindTexture(GL_TEXTURE_2D, Texture);
				GL_CHECKER
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, Manager->BitmapIndices);
				GL_CHECKER
				glUseProgram(0);
				GL_CHECKER	
				glDisableVertexAttribArray(Shader->PositionLocation);
				GL_CHECKER
				glDisableVertexAttribArray(Shader->UVLocation);
				GL_CHECKER
				glDisable(GL_BLEND);
				GL_CHECKER
				glDeleteTextures(1, &Texture);
				GL_CHECKER
			} break;
			case RenderCommandEntryType_DrawRect:{
//				Error("Rendering DrawRect command");
				render_command_entry_drawrect * Command = (render_command_entry_drawrect *)EntryAt;
				EntryAt += sizeof(render_command_entry_drawrect);

				/* TODO(furkan) : This command is not implemented yet */	
			} break;
			case RenderCommandEntryType_Clear:{
//				Error("Rendering Clear command");
				render_command_entry_clear * Command = (render_command_entry_clear *)EntryAt;
				EntryAt += sizeof(render_command_entry_clear);
				
				v3 ClearColor = Command->Color;
				glClearColor(ClearColor.r, ClearColor.g, ClearColor.b, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
			} break;
			InvalidDefaultCase;
		}
	}

	eglSwapBuffers(Manager->Display, Manager->Surface);	
}
