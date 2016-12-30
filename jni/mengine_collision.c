static void
AddCollisionEntry(collision_table * Table, v2 CollisionVector, 
					entity * EntityA, entity * EntityB){	
	u32 EntryIndex;
	for(EntryIndex=0;
		EntryIndex<Table->EntryCount;
		EntryIndex++){
		collision_entry * Entry = Table->Entries + EntryIndex;
		if((Entry->EntityA == EntityA &&
			Entry->EntityB == EntityB) || 
			(Entry->EntityA == EntityB &&
			Entry->EntityB == EntityA)){
			break;
		}
	}

	if(EntryIndex == Table->EntryCount){
		/* NOTE(furkan) : Entry does not exist in table, add it */
		Assert(Table->EntryCount < CollisionTableCapacity);

		collision_entry * Entry = Table->Entries + EntryIndex;
		Entry->EntityA = EntityA;
		Entry->EntityB = EntityB;
		/*	NOTE(furkan) : Collision Vector is PosA-PosB 
			most of the time, but not all the time
		*/
		Entry->CollisionVector = CollisionVector;
		Table->EntryCount++;
	}
}

static void
ReflectVelocity(entity * Entity, v2 CollisionVector){
	component_rigid_body * RigidBody = GetComponent(Entity,
										ComponentType_RigidBody);
	v2 Position = Entity->Transform.Position;

	v2 OldVelocity = RigidBody->Velocity;
	v2 NewVelocity = ScalarMulV2(
				LengthV2(OldVelocity),
			    NormaliseV2(CollisionVector));

	r32 VelocityMagnitudeY
		= GetVelocityAt(Position.y, 5.30f, 
						GravityAcceleration);

	if((NewVelocity.x < 0.0f && 
		OldVelocity.x < 0.0f) ||
	   (NewVelocity.x > 0.0f &&
	    OldVelocity.x > 0.0f)){
		NewVelocity.x = OldVelocity.x;
	} else if(NewVelocity.x == 0.0f){
		NewVelocity.x = OldVelocity.x;
	} else {
		NewVelocity.x = -OldVelocity.x;
	}
	NewVelocity.y = (NewVelocity.y < 0.0f) ?
					-VelocityMagnitudeY :
					 VelocityMagnitudeY;
	RigidBody->Velocity = NewVelocity;
}

static void
HandleCharacterCharacterCollision(entity * CharacterA,
									entity * CharacterB,
									v2 CollisionVector){
	ReflectVelocity(CharacterA, CollisionVector);
	ReflectVelocity(CharacterB, ScalarMulV2(-1.0f, CollisionVector));
}

static void EndGame(game_memory * Memory);

static void
HandlePlayerEnemyCollision(game_memory * Memory, 
							entity * Player, entity * Enemy, 
								v2 CollisionVector){
	HandleCharacterCharacterCollision(Player, Enemy, CollisionVector);
	
	component_health * Health = (component_health * ) GetComponent(Player, 
												ComponentType_Health); 

	component_attack * Attack = (component_attack * ) GetComponent(Enemy, 
												ComponentType_Attack);
	
	if(Health && Attack){
		Warning("Damage! %f-%f = %f", 
							Health->Current, Attack->HitPoint,
							Health->Current - Attack->HitPoint);
		Health->Current -= Attack->HitPoint;
		if(Health->Current <= 0.0f){
			Warning("Game over!");
			EndGame(Memory);
		}
	}
}

static void
HandleGroundCollision(entity * Entity, v2 CollisionVector){
	ReflectVelocity(Entity, CollisionVector);
}

static void
HandleCollisions(game_memory * Memory, r32 DeltaTime){
	collision_table * Table = &Memory->CollisionTable;

	u32 EntryIndex;
	for(EntryIndex=0;
		EntryIndex<Table->EntryCount;
		EntryIndex++){
		collision_entry * Entry = Table->Entries + EntryIndex;
		entity * EntityA = Entry->EntityA;
		entity * EntityB = Entry->EntityB;
		v2 CollisionVector = Entry->CollisionVector;

		switch(EntityA->Type){
			case EntityType_Background: {
				switch(EntityB->Type){
					case EntityType_Background: {
						/* Do nothing */
					} break;
					case EntityType_Player: {
						HandleGroundCollision(EntityB, 
									ScalarMulV2(-1.0f, CollisionVector));
					} break;
					case EntityType_Enemy: {
						HandleGroundCollision(EntityB, 
									ScalarMulV2(-1.0f, CollisionVector));
					} break;
				}
			} break;
			case EntityType_Player: {
				switch(EntityB->Type){
					case EntityType_Background: {
						HandleGroundCollision(EntityA, CollisionVector);
					} break;
					case EntityType_Player: {
						/* Do nothing */
					} break;
					case EntityType_Enemy: {
						HandlePlayerEnemyCollision(Memory, 
													EntityA, EntityB, 
													CollisionVector);
					} break;
				}
			} break;
			case EntityType_Enemy: {
				switch(EntityB->Type){
					case EntityType_Background: {
						HandleGroundCollision(EntityA, CollisionVector);
					} break;
					case EntityType_Player: {
						HandlePlayerEnemyCollision(Memory, 
									EntityB, EntityA, 
									ScalarMulV2(-1.0f, CollisionVector));
					} break;
					case EntityType_Enemy: {
						HandleCharacterCharacterCollision(EntityA, EntityB,
														CollisionVector);
					} break;
				}		
			} break;
		}
	}	
}
