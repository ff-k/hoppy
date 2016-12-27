#define GravityAcceleration -9.8f

typedef struct rect{
	v2 Size;
} rect;

typedef struct circle{
	r32 Radius;
} circle;

typedef struct rect_circle_sum {
	v2 Vertices[8];
} rect_circle_sum;

inline rect_circle_sum
RectCircleSum(rect Rect, v2 RectPosition, circle Circle){
	rect_circle_sum Result;

	r32 RectWidthHalf = (Rect.Size.x/2.0f);
	r32 RectHeightHalf = (Rect.Size.y/2.0f);

	Result.Vertices[0] = V2(
						RectPosition.x - 
						RectWidthHalf - 
						Circle.Radius,
						RectPosition.y + 
						RectHeightHalf);

	Result.Vertices[1] = V2(
						RectPosition.x - 
						RectWidthHalf,
						RectPosition.y + 
						RectHeightHalf + 
						Circle.Radius);

	Result.Vertices[2] = V2(
						RectPosition.x + 
						RectWidthHalf,
						RectPosition.y + 
						RectHeightHalf + 
						Circle.Radius);

	Result.Vertices[3] = V2(
						RectPosition.x + 
						RectWidthHalf + 
						Circle.Radius,
						RectPosition.y + 
						RectHeightHalf);

	Result.Vertices[4] = V2(
						RectPosition.x + 
						RectWidthHalf + 
						Circle.Radius,
						RectPosition.y - 
						RectHeightHalf);

	Result.Vertices[5] = V2(
						RectPosition.x + 
						RectWidthHalf,
						RectPosition.y - 
						RectHeightHalf -
						Circle.Radius);
	
	Result.Vertices[6] = V2(
						RectPosition.x - 
						RectWidthHalf,
						RectPosition.y - 
						RectHeightHalf -
						Circle.Radius);

	Result.Vertices[7] = V2(
						RectPosition.x - 
						RectWidthHalf - 
						Circle.Radius,
						RectPosition.y - 
						RectHeightHalf);

	return Result;
}

inline b32 
IsPointInCircle(circle Circle, v2 CirclePosition, v2 Point){
	b32 Result = false;

	r32 Radius = Square(Point.x-CirclePosition.x) + 
					Square(Point.y-CirclePosition.y);
	
	if(Radius <= Square(Circle.Radius)){
		Result = true;
	}
	return Result;
}

inline b32 
IsPointInRect(rect Rect, v2 RectPosition, v2 Point){
	b32 Result = false;

	r32 Top = RectPosition.y + (Rect.Size.y/2.0f);
	r32 Bottom = RectPosition.y - (Rect.Size.y/2.0f);

	r32 Right = RectPosition.x + (Rect.Size.x/2.0f);
	r32 Left = RectPosition.x - (Rect.Size.x/2.0f);

	if(Point.x <= Right && Point.x >= Left){
		if(Point.y <= Top && Point.y >= Bottom){
			Result = true;
		}
	}

	return Result;
}

static r32
GetVelocityAt(r32 PositionAt, r32 PositionMax, r32 Acceleration){
	r32 Result;

	Assert(PositionMax > PositionAt);
	r32 Distance = PositionMax - PositionAt;

	Acceleration = Absolute(Acceleration);
	r32 T = sqrt((Distance * 2.0f) / Acceleration);

	Result = Acceleration * T; 

	return Result;
}
