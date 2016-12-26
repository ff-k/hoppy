#include <math.h>

#define Pi 3.14159265358979323846

typedef struct v2{
	union{
		struct{
			r32 x;
			r32 y;
		};
		struct{
			r32 u;
			r32 v;
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

inline v2 V2(r32 x, r32 y){
	v2 Result;
	Result.x = x;
	Result.y = y;
	return Result;
}

inline v2u V2U(u32 x, u32 y){
	v2u Result;
	Result.x = x;
	Result.y = y;
	return Result;
}

inline v2i V2I(s32 x, s32 y){
	v2i Result;
	Result.x = x;
	Result.y = y;
	return Result;
}

inline v2 AddV2(v2 A, v2 B){
	v2 Result;
	Result.x = A.x + B.x;
	Result.y = A.y + B.y;
	return Result;
}

inline v2u AddV2U(v2u A, v2u B){
	v2u Result;
	Result.x = A.x + B.x;
	Result.y = A.y + B.y;
	return Result;
}

inline v2i AddV2I(v2i A, v2i B){
	v2i Result;
	Result.x = A.x + B.x;
	Result.y = A.y + B.y;
	return Result;
}

inline v2 SubV2(v2 A, v2 B){
	v2 Result;
	Result.x = A.x - B.x;
	Result.y = A.y - B.y;
	return Result;
}

inline v2u SubV2U(v2u A, v2u B){
	v2u Result;
	Result.x = A.x - B.x;
	Result.y = A.y - B.y;
	return Result;
}

inline v2i SubV2I(v2i A, v2i B){
	v2i Result;
	Result.x = A.x - B.x;
	Result.y = A.y - B.y;
	return Result;
}

inline r32 LengthV2(v2 Vector){
	r32 Result = sqrt(Vector.x*Vector.x + Vector.y*Vector.y);
	return Result;
}

inline r32 LengthV2U(v2u Vector){
	r32 Result = sqrt(Vector.x*Vector.x + Vector.y*Vector.y);
	return Result;
}

inline r32 LengthV2I(v2i Vector){
	r32 Result = sqrt(Vector.x*Vector.x + Vector.y*Vector.y);
	return Result;
}

inline v2 NormaliseV2(v2 Vector){
	v2 Result;
	r32 Length = LengthV2(Vector);
	Result.x = Vector.x/Length;
	Result.y = Vector.y/Length;
	return Result;
}

inline v2u NormaliseV2U(v2u Vector){
	v2u Result;
	r32 Length = LengthV2U(Vector);
	Result.x = Vector.x/Length;
	Result.y = Vector.y/Length;
	return Result;
}

inline v2i NormaliseV2I(v2i Vector){
	v2i Result;
	r32 Length = LengthV2I(Vector);
	Result.x = Vector.x/Length;
	Result.y = Vector.y/Length;
	return Result;
}

inline v2 ScalarMulV2(r32 Scalar, v2 Vector){
	v2 Result;
	Result.x = Scalar * Vector.x;
	Result.y = Scalar * Vector.y;
	return Result;
}

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

inline v3 V3(r32 x, r32 y, r32 z){
	v3 Result;
	Result.x = x;
	Result.y = y;
	Result.z = z;
	return Result;
}

inline v3u V3U(u32 x, u32 y, u32 z){
	v3u Result;
	Result.x = x;
	Result.y = y;
	Result.z = z;
	return Result;
}

inline v3i V3I(s32 x, s32 y, s32 z){
	v3i Result;
	Result.x = x;
	Result.y = y;
	Result.z = z;
	return Result;
}

typedef struct v4{
	union{
		struct{
			r32 x;
			r32 y;
			r32 z;
			r32 w;
		};
		struct{
			r32 r;
			r32 g;
			r32 b;
			r32 a;
		};
		struct{
			r32 Width;
			r32 Height;
			r32 Depth;
			r32 Ignored;
		};
		r32 E[4];
	};
} v4;

typedef struct v4u{
	union{
		struct{
			u32 x;
			u32 y;
			u32 z;
			u32 w;
		};
		struct{
			u32 r;
			u32 g;
			u32 b;
			u32 a;
		};
		struct{
			u32 Width;
			u32 Height;
			u32 Depth;
			u32 Ignored;
		};
		u32 E[4];
	};
} v4u;

typedef struct v4i{
	union{
		struct{
			s32 x;
			s32 y;
			s32 z;
			s32 w;
		};
		struct{
			s32 r;
			s32 g;
			s32 b;
			s32 a;
		};
		struct{
			s32 Width;
			s32 Height;
			s32 Depth;
			s32 Ignored;
		};
		s32 E[4];
	};
} v4i;

inline v4 V4(r32 x, r32 y, r32 z, r32 w){
	v4 Result;
	Result.x = x;
	Result.y = y;
	Result.z = z;
	Result.w = w;
	return Result;
}

inline v4u V4U(u32 x, u32 y, u32 z, u32 w){
	v4u Result;
	Result.x = x;
	Result.y = y;
	Result.z = z;
	Result.w = w;
	return Result;
}

inline v4i V4I(s32 x, s32 y, s32 z, s32 w){
	v4i Result;
	Result.x = x;
	Result.y = y;
	Result.z = z;
	Result.w = w;
	return Result;
}

#define Maximum(A,B) ((A) > (B)) ? (A) : (B)
#define Minimum(A,B) ((A) < (B)) ? (A) : (B)
#define Absolute(X)  ((X) >= 0)  ? (X) : (-(X))

#define Square(X) ((X)*(X))
