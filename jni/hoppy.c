#include "hoppy.h"

#include "hoppy_spawner.c"

MEngineInit(GameInit){
	BeginStackTraceBlock;
	Verbose("Initialising the game");

	memsz SpawnerSize = sizeof(spawner) * MaxSpawnerCount;
	Memory->Spawners = Platform.AllocateMemory(SpawnerSize);
	ZeroMemory(Memory->Spawners, SpawnerSize);

	asset_manager * AssetManager = Memory->AssetManager;

	entity * Background = CreateEntity(	Memory,
										EntityType_Background,
										V2(12.80f, 7.20f), 
										V2(6.40f, 3.60f));

	entity * Player = CreateEntity(	Memory, 
									EntityType_Player,
									V2(0.68f, 1.28f), V2(6.40f, 5.30f));

	spawner_params_enemy EnemySpawnerParams;
	EnemySpawnerParams.Size = V2(1.09f, 1.28f);
	EnemySpawnerParams.SpawnPositionMin = V2(-1.2f, 1.28f);
	EnemySpawnerParams.SpawnPositionMax = V2(-1.2f, EnemyPeakPosition);

	EnemySpawnerParams.HorizontalVelocityMin = EnemySpeedHorizontalMin;
	EnemySpawnerParams.HorizontalVelocityMax = EnemySpeedHorizontalMax;

	EnemySpawnerParams.PeakPositionY = EnemyPeakPosition;

	CreateSpawner(Memory->Spawners, &Memory->SpawnerCount,
						EnemySpawnPeriodMin, EnemySpawnPeriodMax,
						SpawnerType_Enemy, &EnemySpawnerParams);

	
	r32 ScreenRight = 12.80f; // TODO(furkan) : OMG!

	EnemySpawnerParams.SpawnPositionMin = V2(ScreenRight + 1.2f, 1.28f);
	EnemySpawnerParams.SpawnPositionMax = V2(ScreenRight + 1.2f, 
												EnemyPeakPosition);

	EnemySpawnerParams.HorizontalVelocityMin = -EnemySpeedHorizontalMin;
	EnemySpawnerParams.HorizontalVelocityMax = -EnemySpeedHorizontalMax;

	CreateSpawner(Memory->Spawners, &Memory->SpawnerCount,
						EnemySpawnPeriodMin, EnemySpawnPeriodMax,
						SpawnerType_Enemy, &EnemySpawnerParams);

	EndStackTraceBlock;
}

MEngineUpdate(GameUpdate){	
	BeginStackTraceBlock;
	
	ProcessInputs(Memory, Input);
	UpdateEntities(Memory, Input->DeltaTime);
	UpdateSpawners(Memory, Input->DeltaTime);

	EndStackTraceBlock;
}
