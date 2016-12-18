#define GravityAcceleration -9.8f

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
