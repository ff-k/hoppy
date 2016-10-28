typedef enum {
	RenderCommandEntryType_Unknown,
	RenderCommandEntryType_DrawRect,
	RenderCommandEntryType_Clear
} render_command_entry_type;

typedef struct render_command_entry{
	render_command_entry_type Type;
} render_command_entry;

typedef struct render_command_entry_drawrect{
	rect Rect;
} render_command_entry_drawrect;

typedef struct render_command_entry_clear{
	v3 Color;
} render_command_entry_clear;

typedef struct render_commands{
	v3 ClearColor;
	render_command_entry * Entries;
	u32 CapacityInBytes;
	u32 EntryCount;
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


