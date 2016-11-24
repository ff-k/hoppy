typedef enum {
	RenderCommandEntryType_Unknown,
	RenderCommandEntryType_Clear,
	RenderCommandEntryType_DrawRect,
	RenderCommandEntryType_DrawBitmap
} render_command_entry_type;

typedef struct render_command_entry{
	render_command_entry_type Type;
} render_command_entry;

typedef struct render_command_entry_drawbitmap{
	v2 Position;
	v2 Size;
	asset_bitmap * Bitmap;
} render_command_entry_drawbitmap;

typedef struct render_command_entry_drawrect{
	rect Rect;
	v4 Color;
} render_command_entry_drawrect;

typedef struct render_command_entry_clear{
	v3 Color;
} render_command_entry_clear;

typedef struct render_commands{
	v3 ClearColor;
	u8 * Entries;
	u32 CapacityInBytes;
	u32 EntryAt;
} render_commands;

typedef struct framebuffer{
	/* NOTE(furkan) : Software renderer expects pixels to be
		in 0xAABBGGRR format 
	*/
	/* TODO(furkan) : Currently, Data member assumes that the first byte
		contains the pixel which is at the top-left corner of the screen
		and the format is 0xAABBGGRR. Make them selectable.
	*/
	void * Data;
	memsz Size;

	u32 Width;
	u32 Height;

	u32 Stride;
} framebuffer;


