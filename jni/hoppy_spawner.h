#define MaxSpawnerCount 8

typedef enum {
	SpawnerType_None,

	SpawnerType_Enemy
} spawner_type;

typedef struct spawner_params_enemy {
	v2 Size;
	v2 SpawnPositionMin;
	v2 SpawnPositionMax;
	
	r32 HorizontalVelocityMin;
	r32 HorizontalVelocityMax;

	r32 PeakPositionY;
} spawner_params_enemy;

typedef struct spawner{
	void * Params;

	r32 Counter;
	r32 Period;
	r32 PeriodMin;
	r32 PeriodMax;

	spawner_type Type;
} spawner;
