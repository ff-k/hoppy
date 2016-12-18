static entity * 
CreateEntity(game_memory * Memory, entity_type Type,
				v2 Dimension, v2 Position){
	BeginStackTraceBlock;
	entity * Entity = Platform.AllocateMemory(sizeof(entity));
	ZeroStruct(*Entity);

	entity * EntitySentinel = &Memory->EntitySentinel;

	Entity->Next = EntitySentinel->Next;
	Entity->Next->Prev = Entity;

	EntitySentinel->Next = Entity;
	Entity->Prev = EntitySentinel;

	Entity->Dimension = Dimension;
	Entity->Transform.Position = Position;
	Entity->Transform.Scale = V2(1.0f, 1.0f);
	Entity->IsEnabled = true;

	Entity->ComponentSentinel.Next = &Entity->ComponentSentinel;

	Entity->Type = Type;
	switch(Type){
		case EntityType_Background: {
			asset_manager * AssetManager = Memory->AssetManager;
			Entity->BitmapIndex = LoadBitmap(AssetManager, "Background");
			Assert(Entity->BitmapIndex > 0);
			Entity->IsVisible = true;
		} break;
		case EntityType_Player: {
			asset_manager * AssetManager = Memory->AssetManager;
			Entity->BitmapIndex = LoadBitmap(AssetManager, "Character");
			Assert(Entity->BitmapIndex > 0);
			component_rigid_body * RigidBody;
			RigidBody = AddComponent(Entity, 
									ComponentType_RigidBody)->Structure;
			RigidBody->Velocity = V2(0.0f, 0.0f);
			RigidBody->Acceleration = V2(0.0f, GravityAcceleration);
			RigidBody->Drag = V2(0.97f, 1.0f);
			Entity->IsVisible = true;
		} break;
		case EntityType_Enemy: {
			asset_manager * AssetManager = Memory->AssetManager;
			Entity->BitmapIndex = LoadBitmap(AssetManager, "Enemy");
			Assert(Entity->BitmapIndex > 0);
			component_rigid_body * RigidBody;
			RigidBody = AddComponent(Entity, 
									ComponentType_RigidBody)->Structure;
			RigidBody->Velocity = V2(0.0f, 0.0f);
			RigidBody->Acceleration = V2(0.0f, GravityAcceleration);
			RigidBody->Drag = V2(1.0f, 1.0f);
			Entity->IsVisible = true;
		} break;
		InvalidDefaultCase;
	}
	
	Memory->EntityCount++;

	EndStackTraceBlock;
	return Entity;
}

static void
MoveEntity(entity * Entity, r32 DeltaTime){
	component_rigid_body * RigidBody = GetComponent(Entity, 
												ComponentType_RigidBody); 
	if(RigidBody){	
		v2 Acc = RigidBody->Acceleration;

		Entity->Transform.Position.x += (0.5f*Acc.x*Square(DeltaTime)) +
										RigidBody->Velocity.x*DeltaTime;
		Entity->Transform.Position.y += (0.5f*Acc.y*Square(DeltaTime)) +
										RigidBody->Velocity.y*DeltaTime;

		RigidBody->Velocity.x += Acc.x * DeltaTime;
		RigidBody->Velocity.y += Acc.y * DeltaTime;
		
		RigidBody->Velocity.x *= RigidBody->Drag.x; 
		RigidBody->Velocity.y *= RigidBody->Drag.y; 
	} else {
		Warning("An entity which has not got a rigid body component tried to move!");
	}
}

static void 
UpdateEntities(game_memory * Memory, r32 DeltaTime){
	BeginStackTraceBlock;

	entity * EntitySentinel = &Memory->EntitySentinel;
	entity * Entity = EntitySentinel->Prev;
	while(Entity != EntitySentinel){

		switch(Entity->Type){
			case EntityType_Player: {
				MoveEntity(Entity, DeltaTime);
				component_rigid_body * RigidBody = GetComponent(Entity, 
												ComponentType_RigidBody); 
				if(RigidBody){	
					/* TODO(furkan) : Collision detection */
					if(Entity->Transform.Position.x < 0){
						Entity->Transform.Position.x = 0;
					} else if (Entity->Transform.Position.x > 12.80f){
						Entity->Transform.Position.x = 12.80f;
					}
	
					if(Entity->Transform.Position.y < 0.64f){
						if(RigidBody->Velocity.y < 0.0f){
							RigidBody->Velocity.y *= -1;
						}
					}
				}
			} break;
			case EntityType_Enemy: {
				component_rigid_body * RigidBody = GetComponent(Entity, 
												ComponentType_RigidBody); 
				if(RigidBody){	
					MoveEntity(Entity, DeltaTime);

					if(Entity->Transform.Position.x < 0.0f){
						if(	RigidBody->Velocity.x < 0.0f){
							RigidBody->Velocity.x *= -1.0f;	
						}				
					} else if (Entity->Transform.Position.x > 12.80f){
						if(	RigidBody->Velocity.x > 0.0f){
							RigidBody->Velocity.x *= -1.0f;	
						}				
					}
					if(Entity->Transform.Position.y < 0.64f){
						RigidBody->Velocity.y *= -1;
					}
				}
			} break;
			case EntityType_Background : {
			} break;
			InvalidDefaultCase;
		}
		Entity = Entity->Prev;
	}
	EndStackTraceBlock;
}
