static void
InitialiseRandomNumberGenerator(){
	srand(time(0));
}

static s32 
RandomS32(){
	s32 Result = rand();
	return Result;
}

static s32
RandomS32Between(s32 Min, s32 Max){
	s32 Range = Max-Min;
	s32 Result = (rand()%Range)+Min;
	return Result;
}

static u32 
RandomU32(){
	u32 Result = rand();
	return Result;
}

static u32
RandomU32Between(u32 Min, u32 Max){
	u32 Range = Max-Min;
	u32 Result = (rand()%Range)+Min;
	return Result;
}

static r32 
RandomR32(){
	r32 Random = (r32)rand()/(r32)RAND_MAX;
	r32 Result = Random * (r32)rand();
	return Result;
}

static r32
RandomR32Between(r32 Min, r32 Max){
	r32 Range = Max-Min;
	r32 Result = (((r32)rand()/(r32)RAND_MAX) * Range) + Min;
	return Result;
}
