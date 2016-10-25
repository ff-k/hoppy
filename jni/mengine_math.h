#include <math.h>

typedef struct v2{
	union{
		struct{
			r32 x;
			r32 y;
		};
		struct{
			r32 Width;
			r32 Height;
		};
		r32 E[2];
	};
} v2;

typedef struct v2u{
	union{
		struct{
			u32 x;
			u32 y;
		};
		struct{
			u32 Width;
			u32 Height;
		};
		u32 E[2];
	};
} v2u;

typedef struct v2i{
	union{
		struct{
			s32 x;
			s32 y;
		};
		struct{
			s32 Width;
			s32 Height;
		};
		s32 E[2];
	};
} v2i;

typedef struct v3{
	union{
		struct{
			r32 x;
			r32 y;
			r32 z;
		};
		struct{
			r32 r;
			r32 g;
			r32 b;
		};
		struct{
			r32 Width;
			r32 Height;
			r32 Depth;
		};
		r32 E[3];
	};
} v3;

typedef struct v3u{
	union{
		struct{
			u32 x;
			u32 y;
			u32 z;
		};
		struct{
			u32 r;
			u32 g;
			u32 b;
		};
		struct{
			u32 Width;
			u32 Height;
			u32 Depth;
		};
		u32 E[3];
	};
} v3u;

typedef struct v3i{
	union{
		struct{
			s32 x;
			s32 y;
			s32 z;
		};
		struct{
			s32 r;
			s32 g;
			s32 b;
		};
		struct{
			s32 Width;
			s32 Height;
			s32 Depth;
		};
		s32 E[3];
	};
} v3i;

typedef struct rect{
	v2 Size;
	u32 Color;

	/* NOTE(furkan) : That vector points to bottom-left of the rectangle */
	v2 Position;
} rect;
