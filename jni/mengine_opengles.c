#include "mengine_opengles.h"

char * BitmapVertexShader ="\
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

char * BitmapFragmentShader ="\
	precision mediump float;\
	\
	varying vec2 vTexture;\
	uniform sampler2D uTexture;\
	uniform vec4 uColor;\
	\
	void main() {\
		gl_FragColor = uColor * texture2D(uTexture, vTexture);\
	}";

char * PolygonVertexShader ="\
	attribute vec3 aPosition;\
	uniform mat4 uProjection;\
	\
	void main(){\
		gl_Position = uProjection * vec4(aPosition, 1.0);\
	}";

char * PolygonFragmentShader ="\
	uniform vec4 uColor;\
	\
	void main(){\
		gl_FragColor = uColor;\
	}";

static void
OpenGLESCacheTexture(opengles_texture_cache * Cache, 
						memsz TextureID, GLuint Texture){
	BeginStackTraceBlock;

	Assert(Cache);
	Assert(Cache->Capacity > 0)
	opengles_texture_entry * Entry;
	if(Cache->TextureCount == Cache->Capacity){
		/* NOTE(furkan) : Find LRU and replace with the new one */
		Entry = Cache->Entries + 0;
		u32 EntryIndex;
		for(EntryIndex=1; 
			EntryIndex < Cache->TextureCount; 
			EntryIndex++){
			if(Cache->Entries[EntryIndex].Timestamp < Entry->Timestamp){
				Entry = Cache->Entries + EntryIndex;
			}	
		}

		glDeleteTextures(1, &Entry->Texture);
	} else {
		Entry = Cache->Entries + Cache->TextureCount;
		Cache->TextureCount++;
		Debug("Cached a texture! Total cached texture count : %d", 
													Cache->TextureCount);
	}

	Entry->TextureID = TextureID;
	Entry->Texture = Texture;

	Entry->Timestamp = ElapsedTimeSinceStartup();

	EndStackTraceBlock;
}
static GLuint
OpenGLESLoadTexture(opengles_manager * Manager,
					asset_bitmap * TextureBitmap){
	BeginStackTraceBlock;
	Assert(TextureBitmap);

	GLuint Texture;

	b32 FoundInCache = false;
	u32 TextureIndex;
	for(TextureIndex=0; 
		TextureIndex<Manager->TextureCache.TextureCount;
		TextureIndex++){
		opengles_texture_entry * Entry = Manager->TextureCache.Entries
														+ TextureIndex;
		if(Entry->TextureID == (memsz)TextureBitmap){
			Texture = Entry->Texture;
			FoundInCache = true;
			break;
		}
	}

	if(!FoundInCache){
		GLuint Format = GL_RGBA;

		/*	TODO(furkan) : What if glGenTextures fail? 
			Should texture cache size be dependent on 
			device's graphics processor?
		*/
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
						Format, GL_UNSIGNED_BYTE, TextureBitmap->Pixels);
		GL_CHECKER
		glBindTexture(GL_TEXTURE_2D, 0);
		GL_CHECKER
		if(glGetError() != GL_NO_ERROR){
			Error("An error occured while loading the texture");
			Texture = -1;
		} else {
			OpenGLESCacheTexture(&Manager->TextureCache, 
								(memsz)TextureBitmap, Texture);
		}
	}
	
	EndStackTraceBlock;
	return Texture;
}

static GLuint
OpenGLESLoadShader(opengles_manager * Manager, 
					const char * VertexShaderSource, 
					const char * FragmentShaderSource){
	BeginStackTraceBlock;
	GLint Result;
	GLchar ShaderResultLog[256];
	GLuint VertexShader, FragmentShader, ShaderProgram;
	b8 Success = true;

	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShader, 1, &VertexShaderSource, 0);
	glCompileShader(VertexShader);
	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Result);
	if (Result == GL_FALSE) {
		Error("Vertex shader compilation failed!");
		glGetShaderInfoLog(VertexShader, sizeof(ShaderResultLog), 0, ShaderResultLog);
		Error("%s", ShaderResultLog);
		Success = false;
	}
	

	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShader, 1, &FragmentShaderSource, 0);
	glCompileShader(FragmentShader);
	glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &Result);
	if (Result == GL_FALSE) {
		Error("Fragment shader compilation failed!");
		glGetShaderInfoLog(FragmentShader, sizeof(ShaderResultLog), 0, ShaderResultLog);
		Error("%s", ShaderResultLog);
		Success = false;
	}

	ShaderProgram = glCreateProgram();
	if(ShaderProgram){
		glAttachShader(ShaderProgram, VertexShader);
		glAttachShader(ShaderProgram, FragmentShader);
		glLinkProgram(ShaderProgram);
		glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Result);
		if (Result == GL_FALSE){
			Error("Shader program could not been linked!");
			glGetProgramInfoLog(ShaderProgram, sizeof(ShaderResultLog), 0, ShaderResultLog);
			Error("%s", ShaderResultLog);
			Success = false;
		}

		glValidateProgram(ShaderProgram);
		glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Result);
		if(Result == GL_FALSE){
			Error("Shader program could not been validated!");
			glGetProgramInfoLog(ShaderProgram, sizeof(ShaderResultLog), 0, ShaderResultLog);
			Error("%s", ShaderResultLog);
			Success = false;
		}

		glDetachShader(ShaderProgram, VertexShader);
		glDetachShader(ShaderProgram, FragmentShader);
	} else {
		Success = false;
	}

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);

	if(!Success){
		ShaderProgram = -1;
	}

	EndStackTraceBlock;
	return ShaderProgram;
}

static void
OpenGLESInitShaders(opengles_manager * Manager){
	BeginStackTraceBlock;
	opengles_bitmap_shader * BitmapShader = &Manager->BitmapShader;
	BitmapShader->Program = OpenGLESLoadShader(Manager, 
												BitmapVertexShader, 
												BitmapFragmentShader);
	Assert(BitmapShader->Program != -1);

	BitmapShader->PositionLocation = glGetAttribLocation(BitmapShader->Program, "aPosition");
	BitmapShader->UVLocation = glGetAttribLocation(BitmapShader->Program, "aTexture");
	BitmapShader->ProjectionLocation = glGetUniformLocation(BitmapShader->Program,"uProjection");
	BitmapShader->TextureLocation = glGetUniformLocation(BitmapShader->Program, "uTexture");
	BitmapShader->ColorLocation = glGetUniformLocation(BitmapShader->Program, "uColor");

	opengles_polygon_shader * PolygonShader = &Manager->PolygonShader;
	PolygonShader->Program = OpenGLESLoadShader(Manager,
												PolygonVertexShader,
												PolygonFragmentShader);
	Assert(PolygonShader->Program != -1);

	PolygonShader->PositionLocation = glGetAttribLocation(PolygonShader->Program, "aPosition");
	PolygonShader->ColorLocation = glGetUniformLocation(PolygonShader->Program, "uColor");
	PolygonShader->ProjectionLocation = glGetUniformLocation(PolygonShader->Program,"uProjection");
	EndStackTraceBlock;
}

static void
OpenGLESInitTextureCache(opengles_manager * Manager){
	if(Manager->TextureCache.Capacity == 0){
		Assert(Manager->TextureCache.Entries == 0);

		opengles_texture_entry * Entries = Platform.AllocateMemory(sizeof(opengles_texture_entry) * GLESTextureCacheCapacity);
		Manager->TextureCache.Entries = Entries;
		Manager->TextureCache.Capacity = GLESTextureCacheCapacity;

		Assert(Manager->TextureCache.TextureCount == 0);
	}
}

static void 
OpenGLESInit(ANativeWindow * Window, opengles_manager * Manager, 
													v2i * ScreenDim){
	BeginStackTraceBlock;
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
							Success &= eglQuerySurface(Manager->Display, Manager->Surface, EGL_WIDTH, &ScreenDim->x);
							Success &= eglQuerySurface(Manager->Display, Manager->Surface, EGL_HEIGHT, &ScreenDim->y);
							Success &= (ScreenDim->Width > 0) && 
										(ScreenDim->Height > 0);
							if (Success){
								glViewport(0, 0, 
											ScreenDim->x, 
											ScreenDim->y);
								glDisable(GL_DEPTH_TEST);
								
								memset(Manager->ProjectionMatrix,
									0, sizeof(Manager->ProjectionMatrix));
								Manager->ProjectionMatrix[0][0] = (PixelsPerUnit * 2.0f) / (GLfloat)ScreenDim->x;
								Manager->ProjectionMatrix[1][1] = (PixelsPerUnit * 2.0f) / (GLfloat)ScreenDim->y;
								Manager->ProjectionMatrix[2][2] = -1.0f;
								Manager->ProjectionMatrix[3][0] = -1.0f;
								Manager->ProjectionMatrix[3][1] = -1.0f;
								Manager->ProjectionMatrix[3][3] =  1.0f;
				
								Manager->RectIndices[0] = 0;
								Manager->RectIndices[1] = 1;
								Manager->RectIndices[2] = 2;
								Manager->RectIndices[3] = 2;
								Manager->RectIndices[4] = 3;
								Manager->RectIndices[5] = 0;

								u32 LastIndex = GLESCircleIndexCount-1;
								u32 Index = 0;
								while(1){
									Manager->CircleIndices[Index] = 0;
									Index++;
									Manager->CircleIndices[Index] = 
															(Index+2)/3;
									Index++;

									if(Index == LastIndex){
										Manager->CircleIndices[Index] = 1;
										break;
									} else {
										Manager->CircleIndices[Index] = 
										 Manager->CircleIndices[Index-1]+1;
									}
									Index++;
								}
								OpenGLESInitShaders(Manager);

								OpenGLESInitTextureCache(Manager);	

								eglSwapInterval(Manager->Display, 1);

								Manager->IsInitialised = true;
								Verbose("OpenGL ES initialised!");
							}
						}
					}		
				}
			}
		}
	}
	EndStackTraceBlock;
}

static void OpenGLESDeleteShader(GLuint Program){
	if(Program != -1){
		glDeleteShader(Program);
	}
}

static void OpenGLESStop(opengles_manager * Manager){
	BeginStackTraceBlock;

	OpenGLESDeleteShader(Manager->BitmapShader.Program);
	OpenGLESDeleteShader(Manager->PolygonShader.Program);

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
	EndStackTraceBlock;
}

static void 
OpenGLESRenderCommands(opengles_manager * Manager, 
						render_commands * Commands){
	BeginStackTraceBlock;
	u8 * EntryAt = (u8 *)Commands->Entries;
	u8 * EntryAtLimit = (u8 *)(Commands->Entries) + Commands->EntryAt;
	s32 Count = 0;
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
		
				r32 Width = Command->Size.x;
				r32 Height = Command->Size.y;

				v4 Position[4];
				Position[0].x = Command->Position.x-Width/2.0f;
				Position[0].y = Command->Position.y+Height/2.0f;
				Position[0].z =  0.0f;
				Position[0].w =  1.0f;
			
				Position[1].x = Command->Position.x+Width/2.0f;
				Position[1].y = Command->Position.y+Height/2.0f;
				Position[1].z =  0.0f;
				Position[1].w =  1.0f;
			
				Position[2].x = Command->Position.x+Width/2.0f;
				Position[2].y = Command->Position.y-Height/2.0f;
				Position[2].z =  0.0f;
				Position[2].w =  1.0f;
			
				Position[3].x = Command->Position.x-Width/2.0f;
				Position[3].y = Command->Position.y-Height/2.0f;
				Position[3].z =  0.0f;
				Position[3].w =  1.0f;

				opengles_bitmap_shader * Shader = &Manager->BitmapShader;

				glUseProgram(Shader->Program);
				GL_CHECKER
				glUniformMatrix4fv(Shader->ProjectionLocation, 1, 
					GL_FALSE, (GLfloat *)Manager->ProjectionMatrix);
				GL_CHECKER
				glUniform1i(Shader->TextureLocation, 0);
				v4 Color = V4(1.0f, 1.0f, 1.0f, 1.0f);
				glUniform4fv(Shader->ColorLocation, 1, (GLfloat *)&Color);
				GL_CHECKER
				glEnableVertexAttribArray(Shader->PositionLocation);
				GL_CHECKER	
				glVertexAttribPointer(Shader->PositionLocation, 4,
					GL_FLOAT, GL_FALSE, sizeof(v4), Position);
				GL_CHECKER
				glEnableVertexAttribArray(Shader->UVLocation);
				GL_CHECKER	

				/*	TODO(furkan) : Pack UV coordinates with texture data.
					Current version will cause a lot pain in the butt
					for texture atlases.
				*/
				v2 UV[4];
				UV[0] = V2(0.0f, 1.0f);
				UV[1] = V2(1.0f, 1.0f);
				UV[2] = V2(1.0f, 0.0f);
				UV[3] = V2(0.0f, 0.0f);
				glVertexAttribPointer(Shader->UVLocation, 2,
					GL_FLOAT, GL_FALSE, sizeof(v2), UV);
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
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, Manager->RectIndices);
				GL_CHECKER
				glUseProgram(0);
				GL_CHECKER	
				glDisableVertexAttribArray(Shader->PositionLocation);
				GL_CHECKER
				glDisableVertexAttribArray(Shader->UVLocation);
				GL_CHECKER
				glDisable(GL_BLEND);
				GL_CHECKER				
			} break;
			case RenderCommandEntryType_DrawRect:{
				render_command_entry_drawrect * Command = (render_command_entry_drawrect *)EntryAt;
				EntryAt += sizeof(render_command_entry_drawrect);
		
				r32 Width = Command->Rect.Size.x;
				r32 Height = Command->Rect.Size.y;

//				Warning("DrawRectPosition : (%f, %f), Size : (%f, %f)",
//								Command->Position.x, Command->Position.y,
//								Width, Height);

				v4 Position[4];
				Position[0].x = Command->Position.x-Width/2.0f;
				Position[0].y = Command->Position.y+Height/2.0f;
				Position[0].z =  0.0f;
				Position[0].w =  1.0f;
			
				Position[1].x = Command->Position.x+Width/2.0f;
				Position[1].y = Command->Position.y+Height/2.0f;
				Position[1].z =  0.0f;
				Position[1].w =  1.0f;
			
				Position[2].x = Command->Position.x+Width/2.0f;
				Position[2].y = Command->Position.y-Height/2.0f;
				Position[2].z =  0.0f;
				Position[2].w =  1.0f;
			
				Position[3].x = Command->Position.x-Width/2.0f;
				Position[3].y = Command->Position.y-Height/2.0f;
				Position[3].z =  0.0f;
				Position[3].w =  1.0f;

				opengles_polygon_shader * Shader = &Manager->PolygonShader;

				glUseProgram(Shader->Program);
				GL_CHECKER
				glUniformMatrix4fv(Shader->ProjectionLocation, 1, 
					GL_FALSE, (GLfloat *)Manager->ProjectionMatrix);

				glUniform4fv(Shader->ColorLocation, 1, (GLfloat *)&Command->Color);
				glEnable(GL_BLEND);
				GL_CHECKER	
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				
				glEnableVertexAttribArray(Shader->PositionLocation);
				GL_CHECKER
				
				glVertexAttribPointer(Shader->PositionLocation, 4, 
					GL_FLOAT, GL_FALSE, sizeof(v4), (GLfloat *)Position);
				GL_CHECKER
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, Manager->RectIndices);
				GL_CHECKER

				glUseProgram(0);
				GL_CHECKER
				glDisable(GL_BLEND);
				GL_CHECKER				
				glDisableVertexAttribArray(Shader->PositionLocation);
				GL_CHECKER
			} break;
			case RenderCommandEntryType_DrawCircle:{
				render_command_entry_drawcircle * Command = (render_command_entry_drawcircle *)EntryAt;
				EntryAt += sizeof(render_command_entry_drawcircle);

				u32 VertexCount = GLESCircleVertexCount;
				v4 Position[GLESCircleVertexCount];

				Position[0].x = Command->Position.x;
				Position[0].y = Command->Position.y;
				Position[0].z = 0.0f;
				Position[0].w = 1.0f;

				r32 AngleStep = (2.0f * Pi)/(r32)(VertexCount-1);
				r32 Angle = 0.0f;

				u32 VertexIndex;
				for(VertexIndex=1;
					VertexIndex < VertexCount;
					VertexIndex++){
					Angle = (VertexIndex-1.0f)*AngleStep;
					Position[VertexIndex].x = Command->Position.x + 
											Command->Radius * cosf(Angle);
					Position[VertexIndex].y = Command->Position.y + 
											Command->Radius * sinf(Angle);
					Position[VertexIndex].z = 0.0f;
					Position[VertexIndex].w = 1.0f;	
				}

				opengles_polygon_shader * Shader 
											= &Manager->PolygonShader;

				glUseProgram(Shader->Program);
				glUniformMatrix4fv(Shader->ProjectionLocation, 1, 
					GL_FALSE, (GLfloat *)Manager->ProjectionMatrix);

				glUniform4fv(Shader->ColorLocation, 1, 
										(GLfloat *)&Command->Color);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				
				glEnableVertexAttribArray(Shader->PositionLocation);
				glVertexAttribPointer(Shader->PositionLocation, 
										4, 
										GL_FLOAT, GL_FALSE, sizeof(v4), 
										(GLfloat *)Position);
				glDrawElements(GL_TRIANGLES, GLESCircleIndexCount, 
								GL_UNSIGNED_SHORT, Manager->CircleIndices);
				glUseProgram(0);
				glDisable(GL_BLEND);
				glDisableVertexAttribArray(Shader->PositionLocation);
			} break;
			case RenderCommandEntryType_DrawString:{
//				Error("Rendering DrawString command");
				render_command_entry_drawstring * Command = (render_command_entry_drawstring *)EntryAt;
				EntryAt += sizeof(render_command_entry_drawstring);

				/*	TODO(furkan) : This branch needs to 
					be reimplemented.
				*/
				r32 Width = Command->Height * 0.75f;
				r32 Height = Command->Height;

				v4 Position[4];
				Position[0].x = Command->Position.x-Width/2.0f;
				Position[0].y = Command->Position.y+Height/2.0f;
				Position[0].z =  0.0f;
				Position[0].w =  1.0f;
			
				Position[1].x = Command->Position.x+Width/2.0f;
				Position[1].y = Command->Position.y+Height/2.0f;
				Position[1].z =  0.0f;
				Position[1].w =  1.0f;
			
				
				Position[2].y = Command->Position.y-Height/2.0f;
				Position[2].z =  0.0f;
				Position[2].w =  1.0f;
			
				Position[3].x = Command->Position.x-Width/2.0f;
				Position[3].y = Command->Position.y-Height/2.0f;
				Position[3].z =  0.0f;
				Position[3].w =  1.0f;

				/*	TODO(furkan) : Pack UV coordinates with texture data.
					Current version will cause a lot pain in the butt
					for texture atlases.
				*/
				v2 UV[4];
				UV[0] = V2(0.0f, 1.0f);
				UV[1] = V2(1.0f, 1.0f);
				UV[2] = V2(1.0f, 0.0f);
				UV[3] = V2(0.0f, 0.0f);

				opengles_bitmap_shader * Shader = &Manager->BitmapShader;
				glUseProgram(Shader->Program);

				glUniformMatrix4fv(Shader->ProjectionLocation, 1, 
					GL_FALSE, (GLfloat *)Manager->ProjectionMatrix);

				glUniform1i(Shader->TextureLocation, 0);
				glUniform4fv(Shader->ColorLocation, 1, 
										(GLfloat *)&Command->Color);

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				u32 CharIndex = 0;
				while(Command->String[CharIndex]){
					u8 Char = Command->String[CharIndex];

					Position[3].x = Position[0].x = Command->Position.x +
													(Width*CharIndex) -
													Width/2.0f;
					Position[2].x = Position[1].x = Command->Position.x +
													(Width*CharIndex) +
													Width/2.0f;

					glEnableVertexAttribArray(Shader->PositionLocation);
					glVertexAttribPointer(Shader->PositionLocation, 4,
							GL_FLOAT, GL_FALSE, sizeof(v4), Position);				
					glEnableVertexAttribArray(Shader->UVLocation);
					glVertexAttribPointer(Shader->UVLocation, 2,
									GL_FLOAT, GL_FALSE, sizeof(v2), UV);

					GLuint Texture = -1;
					
					if(Char >= '0' && Char <= '9'){
						Texture = OpenGLESLoadTexture(Manager, 
								Command->Font->Numeric[Char - '0']);
					} else if(Char >= 'A' && Char <= 'Z'){
						Texture = OpenGLESLoadTexture(Manager, 
								Command->Font->LettersUpper[Char - 'A']);
					} else if(Char >= 'z' && Char <= 'z'){
						Texture = OpenGLESLoadTexture(Manager, 
								Command->Font->LettersLower[Char - 'a']);
					}

					if(Texture != -1){
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, Texture);
						glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT,
												Manager->RectIndices);
					}

					CharIndex++;
				}

				glUseProgram(0);
				glDisableVertexAttribArray(Shader->PositionLocation);
				glDisableVertexAttribArray(Shader->UVLocation);
				glDisable(GL_BLEND);
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
	EndStackTraceBlock;
}
