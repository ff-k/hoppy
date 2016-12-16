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
			AddComponent(Entity, ComponentType_RigidBody);
			Entity->IsVisible = true;
		} break;
		case EntityType_Enemy: {
			asset_manager * AssetManager = Memory->AssetManager;
			Entity->BitmapIndex = LoadBitmap(AssetManager, "Enemy");
			Assert(Entity->BitmapIndex > 0);
			AddComponent(Entity, ComponentType_RigidBody);
			Entity->IsVisible = true;
		} break;
		InvalidDefaultCase;
	}
	
	Memory->EntityCount++;

	EndStackTraceBlock;
	return Entity;
}

#define MaxDistanceFromGround 5.12f

static void 
UpdateEntities(game_memory * Memory, game_input * Input){
	BeginStackTraceBlock;
	/*	NOTE(furkan) : 
		Entites[0] is reserved for null-entity
	*/
	entity * EntitySentinel = &Memory->EntitySentinel;
	entity * Entity = EntitySentinel->Prev;
	while(Entity != EntitySentinel){

		switch(Entity->Type){
			case EntityType_Player: {
				component_rigid_body * RigidBody = GetComponent(Entity,
												ComponentType_RigidBody);
				Assert(RigidBody != 0);
				r32 Speed = 0.0025f; // RigidBody->Velocity.y;
				static r32 DistanceFactor = 0.0f;
				r32 DistanceFromGround = sinf(DistanceFactor) * MaxDistanceFromGround;
				DistanceFactor += Speed*Input->DeltaTime;
				Entity->Transform.Position.y = DistanceFromGround;
				if(DistanceFromGround<0.0f){
					Entity->Transform.Position.y *= -1.0f;
				}
				Entity->Transform.Position.y += 0.8f;

				v2 Screen = V2(	Memory->GLESManager->ScreenDim.x,
								Memory->GLESManager->ScreenDim.y);
				if(Input->PointerCount){
					if(Input->PointerCoordinates[0].x > Screen.x/2.0f){
						Entity->Transform.Position.x += 0.1f;
					} else {
						Entity->Transform.Position.x -= 0.1f;
					}
				}
#define MovementLimitLeft (Entity->Dimension.x/2.0f)
#define MovementLimitRight (Screen.x/PixelsPerUnit-Entity->Dimension.x/2.0f)

				if(Entity->Transform.Position.x < MovementLimitLeft){
					Entity->Transform.Position.x = MovementLimitLeft;
				} else if(Entity->Transform.Position.x > MovementLimitRight){
					Entity->Transform.Position.x = MovementLimitRight;
				}

			} break;
			case EntityType_Enemy: {
				r32 Speed = 0.0025f; // RigidBody->Velocity.y;
				static r32 DistanceFactor = 0.0f;
				r32 DistanceFromGround = sinf(DistanceFactor) * MaxDistanceFromGround;
				DistanceFactor += Speed*Input->DeltaTime;
				Entity->Transform.Position.y = DistanceFromGround;
				if(DistanceFromGround<0.0f){
					Entity->Transform.Position.y *= -1.0f;
				}
				Entity->Transform.Position.y += 0.8f;

				v2 Screen = V2(	Memory->GLESManager->ScreenDim.x,
								Memory->GLESManager->ScreenDim.y);

#define MovementLimitLeft (Entity->Dimension.x/2.0f)
#define MovementLimitRight (Screen.x/PixelsPerUnit-Entity->Dimension.x/2.0f)

				if(Entity->Transform.Position.x < MovementLimitLeft){
					Entity->Transform.Position.x = MovementLimitLeft;
				} else if(Entity->Transform.Position.x > MovementLimitRight){
					Entity->Transform.Position.x = MovementLimitRight;
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
