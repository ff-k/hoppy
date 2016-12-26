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

			/*	TODO(furkan) : Create a floor entity and 
				seperate collider.
			*/
			v2 ColliderSize = V2((r32)Memory->ScreenDim.x, 
									0.96f);
			
			component_collider * Collider = AddCollider(Entity, 
													ColliderType_Rect,
													&ColliderSize);
			Collider->Offset = V2(0.0f,
						ColliderSize.y/2.0f - Entity->Dimension.y/2.0f);
			
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
			
			r32 ColliderRadius = Maximum(Dimension.x, Dimension.y) / 2.0f;
			component_collider * Collider = AddCollider(Entity, 
													ColliderType_Circle,
													&ColliderRadius);

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
			
			r32 ColliderRadius = Maximum(Dimension.x, Dimension.y) / 2.0f;
			component_collider * Collider = AddCollider(Entity, 
													ColliderType_Circle,
													&ColliderRadius);
			
			Entity->IsVisible = true;
		} break;
		InvalidDefaultCase;
	}
	
	Memory->EntityCount++;

	EndStackTraceBlock;
	return Entity;
}

static void
MoveEntity(entity * EntitySentinel, entity * Entity, r32 DeltaTime){
	if(Entity->Collided){
		return;
	}
	BeginStackTraceBlock;

	component_rigid_body * RigidBody = GetComponent(Entity, 
												ComponentType_RigidBody); 
	if(RigidBody){
		v2 Acc = RigidBody->Acceleration;

		r32 HalfDeltaTimeSq = 0.5f*Square(DeltaTime);
		v2 FinalPosition;
		FinalPosition.x = Entity->Transform.Position.x + 
						(Acc.x*HalfDeltaTimeSq) + 
						RigidBody->Velocity.x*DeltaTime;
		FinalPosition.y = Entity->Transform.Position.y + 
						(Acc.y*HalfDeltaTimeSq) +
						RigidBody->Velocity.y*DeltaTime;

		b32 HitCollider = false;
		component_collider * Collider;
		Collider = (component_collider *)GetComponent(Entity, 
												ComponentType_Collider);

		u32 IterationCount = 8;

		v2 Position = Entity->Transform.Position;
		v2 Step = V2((FinalPosition.x-Position.x) /(float)IterationCount,
					 (FinalPosition.y-Position.y) /(float)IterationCount);
		
		u32 IterationIndex;
		for(IterationIndex = 0;
			IterationIndex < IterationCount;
			IterationIndex++){
			entity * Other = EntitySentinel->Prev;
			while(Other != EntitySentinel){
				if(Other != Entity){
					component_collider * OtherCollider;
					OtherCollider = (component_collider *)GetComponent(Other, ComponentType_Collider);
					if(OtherCollider){
						if( (Collider->Type == ColliderType_Rect &&
							 OtherCollider->Type == ColliderType_Rect)){
							/* NOTE(furkan) : Rect-Rect collision */
							Warning("Rect-rect collision testing");
						} else if((Collider->Type == ColliderType_Circle &&	
							OtherCollider->Type == ColliderType_Circle)){
							/* NOTE(furkan) : Circle-Circle collision */
							circle Sum;
							Sum.Radius = Collider->Radius + 
											OtherCollider->Radius;

							v2 EntityColliderPosition = AddV2(Position,
														Collider->Offset);
							v2 OtherColliderPosition = 
											AddV2(Other->Transform.Position, 
													OtherCollider->Offset);
							if(IsPointInCircle(Sum, 
												OtherColliderPosition,
												EntityColliderPosition)){	
								v2 CollisionVector = SubV2(EntityColliderPosition,
														OtherColliderPosition);
								
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
								} else {
									NewVelocity.x = -OldVelocity.x;
								}
								NewVelocity.y = (NewVelocity.y < 0.0f) ?
												-VelocityMagnitudeY :
												 VelocityMagnitudeY;
								RigidBody->Velocity = NewVelocity;
								HitCollider = true;
							}
						} else if((Collider->Type == ColliderType_Circle &&	
								OtherCollider->Type == ColliderType_Rect) || 
								(Collider->Type == ColliderType_Rect &&
							OtherCollider->Type == ColliderType_Circle)){
							/* NOTE(furkan) : Circle-Rect collision */
							Warning("Circle-rect collision testing");
						} else {
							Warning("Undefined collision check between %d and %d", Collider->Type, OtherCollider->Type);
						}
					}
				}
				Other = Other->Prev;
			}

			if(!HitCollider){
				Entity->Transform.Position = Position;
			} else {	
//				Entity->Collided = true;
				break;
			}

			Position.x += Step.x;
			Position.y += Step.y;
		}


		RigidBody->Velocity.x += Acc.x * DeltaTime;
		RigidBody->Velocity.y += Acc.y * DeltaTime;
		
		RigidBody->Velocity.x *= RigidBody->Drag.x; 
		RigidBody->Velocity.y *= RigidBody->Drag.y; 
	} else {
		Warning("An entity which has not got a rigid body component tried to move!");
	}

	EndStackTraceBlock;
}

static void 
UpdateEntities(game_memory * Memory, r32 DeltaTime){
	BeginStackTraceBlock;

	entity * EntitySentinel = &Memory->EntitySentinel;
	entity * Entity = EntitySentinel->Prev;
	while(Entity != EntitySentinel){

		switch(Entity->Type){
			case EntityType_Player: {
				MoveEntity(EntitySentinel, Entity, DeltaTime);
				component_rigid_body * RigidBody = GetComponent(Entity, 
												ComponentType_RigidBody); 
				if(RigidBody){	
					/* TODO(furkan) : Collision detection */
					if(Entity->Transform.Position.x < 0){
						Entity->Transform.Position.x = 0;
					} else if (Entity->Transform.Position.x > 12.80f){
						Entity->Transform.Position.x = 12.80f;
					}
	
					if(Entity->Transform.Position.y < 0.96f){
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
					MoveEntity(EntitySentinel, Entity, DeltaTime);

					if(Entity->Transform.Position.x < 0.0f){
						if(	RigidBody->Velocity.x < 0.0f){
//							RigidBody->Velocity.x *= -1.0f;	
						}				
					} else if (Entity->Transform.Position.x > 12.80f){
						if(	RigidBody->Velocity.x > 0.0f){
//							RigidBody->Velocity.x *= -1.0f;	
						}				
					}
					if(Entity->Transform.Position.y < 0.96f){
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
