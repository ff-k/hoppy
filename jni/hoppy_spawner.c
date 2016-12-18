static spawner *
CreateSpawner(spawner * Spawners, u32 * SpawnerCount, 
					r32 PeriodMin, r32 PeriodMax,
					spawner_type Type, void * Params){
	spawner * Spawner = 0;

	if(*SpawnerCount < MaxSpawnerCount){
		Spawner = Spawners + (*SpawnerCount);

		Spawner->Counter = 0.0f;
		Spawner->Period = RandomR32Between(PeriodMin, PeriodMax);
		Spawner->PeriodMin = PeriodMin;
		Spawner->PeriodMax = PeriodMax;

		Spawner->Params = 0;
		Spawner->Type = Type;

		switch(Type){
			case SpawnerType_Enemy:{
				Spawner->Params = Platform.AllocateMemory(sizeof(spawner_params_enemy));
				*((spawner_params_enemy *)Spawner->Params) = 
										*((spawner_params_enemy *)Params);
			} break;
			InvalidDefaultCase;
		}

		(*SpawnerCount)++;
	} else {
		Error("Spawner container is full! Spawner will not be created!");
	}
	
	return Spawner;
}

static void
Spawn(game_memory * Memory, spawner * Spawner){
	BeginStackTraceBlock;

	switch(Spawner->Type){
		case SpawnerType_Enemy: {
			spawner_params_enemy * Params;
			Params = (spawner_params_enemy *)Spawner->Params;

			v2 SpawnAt;
			SpawnAt.x = RandomR32Between(Params->SpawnPositionMin.x,
										 Params->SpawnPositionMax.x);
			SpawnAt.y = RandomR32Between(Params->SpawnPositionMin.y,
										 Params->SpawnPositionMax.y);

			entity * Enemy = CreateEntity(Memory, EntityType_Enemy,
										Params->Size, SpawnAt);

			component_rigid_body * RigidBody;
			RigidBody = (component_rigid_body *)GetComponent(Enemy,
									ComponentType_RigidBody);

			RigidBody->Velocity.x
						= RandomR32Between(Params->HorizontalVelocityMin,
										Params->HorizontalVelocityMax);

			RigidBody->Velocity.y = GetVelocityAt(SpawnAt.y, 
													Params->PeakPositionY,
													GravityAcceleration);
			Warning("Spawed at (%f, %f) with velocity (%f, %f)",
						SpawnAt.x, SpawnAt.y, 
						RigidBody->Velocity.x, RigidBody->Velocity.y);
		} break;
		InvalidDefaultCase;
	}

	EndStackTraceBlock;
}

static void
UpdateSpawners(game_memory * Memory, r32 DeltaTime){
	BeginStackTraceBlock;

	u32 SpawnerCount = Memory->SpawnerCount;
	spawner * Spawners = Memory->Spawners;

	u32 SpawnerIndex;
	for(SpawnerIndex=0; SpawnerIndex<SpawnerCount; SpawnerIndex++){
		spawner * Spawner = Spawners + SpawnerIndex;
		Spawner->Counter += DeltaTime;
		if(Spawner->Counter >= Spawner->Period){
			Spawn(Memory, Spawner);
			Spawner->Counter = 0.0f;
			Spawner->Period = RandomR32Between(Spawner->PeriodMin,
												Spawner->PeriodMax);
		}
	}
	EndStackTraceBlock;
}
