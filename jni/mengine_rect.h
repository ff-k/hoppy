#ifndef INCLUDED_RECT
#define INCLUDED_RECT

typedef struct rect{
	u32 Width;
	u32 Height;
	u32 Color;

	/* TODO(furkan) : Change them with V2(x,y) */
	/* NOTE(furkan) : That vector points to bottom-left of the rectangle */
	s32 X;
	s32 Y;
} rect;

#endif
