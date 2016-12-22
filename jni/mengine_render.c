#define PushRenderCommandCase(type) \
	Assert((Commands->CapacityInBytes -\
	 		Commands->EntryAt +\
			sizeof(type)) >= 0);\
	type * Cmd = (type *)(Commands->Entries + Commands->EntryAt);\
	*Cmd = *((type *)Params);\
	Commands->EntryAt += sizeof(type);
//	Debug("After command : %d, %p", Commands->EntryAt, Cmd);
static void
PushRenderCommand(render_commands * Commands, 
				  render_command_entry_type Type, 
				  void * Params){
	BeginStackTraceBlock;
	/* TODO(furkan) : Make the render command buffer grow
		when its size is not large enough to store new command
	*/
	Assert((Commands->CapacityInBytes -
	 		Commands->EntryAt +
			sizeof(render_command_entry)) >= 0);
	render_command_entry * Entry = (render_command_entry *)(Commands->Entries + Commands->EntryAt);
//	Debug("Before : %d, %p", Commands->EntryAt, Entry);
	Entry->Type = Type;
	Commands->EntryAt += sizeof(render_command_entry);
//	Debug("After header : %d, %p", Commands->EntryAt, Entry);
	switch(Type){
		case RenderCommandEntryType_DrawBitmap:{
			PushRenderCommandCase(render_command_entry_drawbitmap);
		} break;
		case RenderCommandEntryType_DrawRect:{
//			Debug("Pushing a DrawRect command");
			PushRenderCommandCase(render_command_entry_drawrect);
		} break;
		case RenderCommandEntryType_Clear:{
//			Debug("Pushing a Clear command");
			PushRenderCommandCase(render_command_entry_clear);
		} break;
		InvalidDefaultCase;
	}
	EndStackTraceBlock;
}

static void
ExtractRenderCommands(render_commands * RenderCommands, 
						entity * EntitySentinel,
						asset_manager * AssetManager){
	BeginStackTraceBlock;
	/* NOTE(furkan) : Clear screen before rendering frames*/
	render_command_entry_clear ClearCommand;
	ClearCommand.Color.r = 0.8f;
	ClearCommand.Color.g = 0.0f;
	ClearCommand.Color.b = 0.6f;

	PushRenderCommand(RenderCommands, 
					  RenderCommandEntryType_Clear, 
					  &ClearCommand);	

	/*	TODO(furkan) : Introduce sorting order and 
		sort render commands accordingly */

	entity * Entity = EntitySentinel->Prev;
	while(Entity != EntitySentinel){
		switch(Entity->Type){
			case EntityType_Background:
			case EntityType_Enemy:
			case EntityType_Player: {
				render_command_entry_drawbitmap DrawBitmap;
				DrawBitmap.Position = Entity->Transform.Position;
				DrawBitmap.Size = Entity->Dimension;
				DrawBitmap.Bitmap = AssetManager->Bitmaps + 
										Entity->BitmapIndex;

				PushRenderCommand(RenderCommands,
					  RenderCommandEntryType_DrawBitmap,
					  &DrawBitmap);
#if 1
				component_collider * Collider;
				Collider = (component_collider *)GetComponent(Entity, 
												ComponentType_Collider);
				
				if(Collider){
					render_command_entry_drawrect DrawRect;
					DrawRect.Position = Entity->Transform.Position;
					if(Collider->Type == ColliderType_Rect){
						DrawRect.Rect.Size = Collider->Size;
					} else {
						DrawRect.Rect.Size = V2(Collider->Radius*2.0f,
												Collider->Radius*2.0f);
					}
					DrawRect.Color = V4(1.0f, 0.0f, 1.0f, 0.5f);
					PushRenderCommand(RenderCommands,
									RenderCommandEntryType_DrawRect,
									&DrawRect);
				}
#endif
			} break;
			InvalidDefaultCase;
		}

		Entity = Entity->Prev;
	}
	EndStackTraceBlock;
}

static void 
SWRenderCommands(framebuffer * Framebuffer, render_commands * Commands){
	BeginStackTraceBlock;
	u8 * EntryAt;
	u8 * EntryAtLimit = ((u8* )Commands->Entries) + 
						Commands->CapacityInBytes;
	for(EntryAt = (u8 *)Commands->Entries; 
		EntryAt<EntryAtLimit;){
		render_command_entry * Entry = (render_command_entry *) EntryAt;
		EntryAt += sizeof(render_command_entry);
		switch(Entry->Type){
			case RenderCommandEntryType_DrawRect:{
				render_command_entry_drawrect * Command = (render_command_entry_drawrect *)EntryAt;
				EntryAt += sizeof(render_command_entry_drawrect);

				rect Rect = Command->Rect;
				if( Command->Position.y >= Framebuffer->Height ||
					Command->Position.x >= Framebuffer->Width || 
					Command->Position.y < 0 ||
					Command->Position.x < 0){
					/*	TODO(furkan) : This clipping must be done using 
						camera position
					*/
					/* NOTE(furkan) : Rect is out of the screen. Do not 
						waste time iterating over it */
					continue;
				}

				u32 * Row = ((u32 *) Framebuffer->Data) + (u32)(Framebuffer->Stride * (Framebuffer->Height - Command->Position.y));

				s32 RowLimit = Minimum(Command->Position.y+Rect.Size.Height, Framebuffer->Height);
				s32 ColLimit = Minimum(Command->Position.x+Rect.Size.Width,
										Framebuffer->Width);
				s32 RowAt;
				s32 ColAt;
				for(RowAt=Command->Position.y; 
					RowAt<RowLimit; 
					RowAt++){
					for(ColAt=Command->Position.x; 
						ColAt<ColLimit; 
						ColAt++){
						/* TODO(furkan) : Alpha blending!
						*/
						Row[ColAt] = //((u32)Rect.Color.a << 24) |
													(255 << 24) |
									 ((u32)Command->Color.b << 16) | 
									 ((u32)Command->Color.g << 8) | 
									  (u32)Command->Color.r;
					}

					Row -= Framebuffer->Stride;
				}
				
			} break;
			case RenderCommandEntryType_Clear:{
				render_command_entry_clear * Command = (render_command_entry_clear *)EntryAt;
				EntryAt += sizeof(render_command_entry_clear);
				
				/* TODO(furkan) : Alpha must be added and colors must be
					represented as v4
				*/
				u32 ClearColor = //((u32) Command->Color.a << 24) |
													(255 << 24) |
								 ((u32) Command->Color.b << 16) |
								 ((u32) Command->Color.g << 8) |
								 ((u32) Command->Color.r << 0);
				memset(Framebuffer->Data, ClearColor, Framebuffer->Size);
			}break;
			InvalidDefaultCase;
		}
	}
	EndStackTraceBlock;
}
