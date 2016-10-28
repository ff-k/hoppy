static void 
SWRenderCommands(framebuffer * Framebuffer, render_commands * Commands){
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
				if(Rect.Position.y >= Framebuffer->Height ||
					Rect.Position.x >= Framebuffer->Width || 
					Rect.Position.y < 0 ||
					Rect.Position.x < 0){
					/* TODO(furkan) : This clipping must be done using 
						camera's position
					*/
					/* NOTE(furkan) : Rect is out of the screen. Do not 
						waste time iterating over it */
					continue;
				}

				u32 * Row = ((u32 *) Framebuffer->Data) + (u32)(Framebuffer->Stride * (Framebuffer->Height - Rect.Position.y));

				s32 RowLimit = Minimum(Rect.Position.y+Rect.Size.Height, 
										Framebuffer->Height);
				s32 ColLimit = Minimum(Rect.Position.x+Rect.Size.Width,
										Framebuffer->Width);
				s32 RowAt;
				s32 ColAt;
				for(RowAt=Rect.Position.y; 
					RowAt<RowLimit; 
					RowAt++){
					for(ColAt=Rect.Position.x; 
						ColAt<ColLimit; 
						ColAt++){
						/* TODO(furkan) : Alpha blending!
						*/
						Row[ColAt] = //((u32)Rect.Color.a << 24) |
													(255 << 24) |
									 ((u32)Rect.Color.b << 16) | 
									 ((u32)Rect.Color.g << 8) | 
									  (u32)Rect.Color.r;
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
}