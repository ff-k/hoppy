static void
PrintEntity(entity * Entity){
	Warning("EntityType : %d", Entity->Type);
	component * Component = Entity->ComponentSentinel.Next;
	while(Component != &Entity->ComponentSentinel){
		Warning("ComponentType : %d", Component->Type);
		Component = Component->Next;
	}

}

static entity *
GetEntity(entity * EntitySentinel, entity_type Type){
	entity * Result = 0;

	entity * Entity = EntitySentinel->Prev;
	while(Entity != EntitySentinel){
		if(Entity->Type == Type){
			Result = Entity;
			break;
		}
		Entity = Entity->Prev;
	}

	return Result;
}

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
			v2 ColliderSize = V2(12.80f, 
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

			AddHealthComponent(Entity, 100.0f);

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
			
			AddAttackComponent(Entity, 25.0f);

			Entity->IsVisible = true;
		} break;
		InvalidDefaultCase;
	}
	
	Memory->EntityCount++;

	EndStackTraceBlock;
	return Entity;
}

static void
ClearEntities(entity * EntitySentinel){
	entity * Entity = EntitySentinel->Next;
	while(Entity != EntitySentinel){
		entity * NextEntity = Entity->Next;

		ClearComponents(&Entity->ComponentSentinel);
		free(Entity);

		Entity = NextEntity;
	}

	EntitySentinel->Next = EntitySentinel->Prev = EntitySentinel;
}

static void
MoveEntity(entity * EntitySentinel, entity * Entity, r32 DeltaTime,
			collision_table * CollisionTable){
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
						v2 CollisionVector = V2(0.0f, 0.0f);
						v2 EntityColliderPosition = AddV2(Position,
													Collider->Offset);
						v2 OtherColliderPosition = 
										AddV2(Other->Transform.Position, 
										OtherCollider->Offset);
						if( (Collider->Type == ColliderType_Rect &&
							 OtherCollider->Type == ColliderType_Rect)){
							/* NOTE(furkan) : Rect-Rect collision */
							rect Sum;
							Sum.Size = AddV2(Collider->Size, 
										OtherCollider->Size);

							if(IsPointInRect(Sum,
												OtherColliderPosition,
												EntityColliderPosition)){
								CollisionVector = SubV2(EntityColliderPosition, OtherColliderPosition);
								HitCollider = true;
							}
						} else if((Collider->Type == ColliderType_Circle &&	OtherCollider->Type == ColliderType_Circle)){
							/* NOTE(furkan) : Circle-Circle collision */
							circle Sum;
							Sum.Radius = Collider->Radius + 
											OtherCollider->Radius;

							if(IsPointInCircle(Sum, 
												OtherColliderPosition,
												EntityColliderPosition)){
								CollisionVector = SubV2(EntityColliderPosition, OtherColliderPosition);
								HitCollider = true;
							}
						} else if((Collider->Type == ColliderType_Circle &&								OtherCollider->Type == ColliderType_Rect) || 								(Collider->Type == ColliderType_Rect &&
							OtherCollider->Type == ColliderType_Circle)){
							/* NOTE(furkan) : Circle-Rect collision */
							rect Rect;
							v2 RectPosition;
							circle Circle;
							v2 CirclePosition;

							if(Collider->Type == ColliderType_Rect){
								Rect.Size = Collider->Size;
								RectPosition = EntityColliderPosition;
								Circle.Radius = OtherCollider->Radius;
								CirclePosition = OtherColliderPosition;
							} else {
								Rect.Size = OtherCollider->Size;
								RectPosition = OtherColliderPosition;
								Circle.Radius = Collider->Radius;
								CirclePosition = EntityColliderPosition;
							}

							rect_circle_sum Sum = RectCircleSum(Rect, 
															RectPosition,
															Circle);

							rect SumBorder;
							SumBorder.Size = V2(Sum.Vertices[3].x -
												Sum.Vertices[0].x,
												Sum.Vertices[1].y -
												Sum.Vertices[6].y);

							v2 TopLeftCirclePosition = 
										V2(Sum.Vertices[1].x, 
											Sum.Vertices[0].y);

							v2 TopRightCirclePosition = 
										V2(Sum.Vertices[2].x, 
											Sum.Vertices[3].y);

							v2 BottomLeftCirclePosition = 
										V2(Sum.Vertices[6].x, 
											Sum.Vertices[7].y);

							v2 BottomRightCirclePosition = 
										V2(Sum.Vertices[5].x, 
											Sum.Vertices[4].y);

							if(!IsPointInRect(SumBorder, 
											RectPosition,
											CirclePosition)){
								// No collision
							} else if(CirclePosition.x < Sum.Vertices[2].x && CirclePosition.x > Sum.Vertices[1].x){
								// Collision 0, 1, 2
								CollisionVector = V2(0.0f, 1.0f);
								if(RectPosition.y > CirclePosition.y){
									CollisionVector.y = -1.0f;
								}
								HitCollider = true;
							} else if(CirclePosition.y < Sum.Vertices[0].y && CirclePosition.y > Sum.Vertices[7].y){
								// Collision 3, 4
								CollisionVector = V2(1.0f, 0.0f);
								if(RectPosition.x > CirclePosition.x){
									CollisionVector.x = -1.0f;
								}
								HitCollider = true;
							} else if(IsPointInCircle(Circle,
												TopLeftCirclePosition,
												CirclePosition)){
								// Collision 5
								CollisionVector = SubV2(EntityColliderPosition, OtherColliderPosition);
								HitCollider = true;
							} else if(IsPointInCircle(Circle,
												TopRightCirclePosition,
												CirclePosition)){
								// Collision 6
								CollisionVector = SubV2(EntityColliderPosition, OtherColliderPosition);
								HitCollider = true;
							} else if(IsPointInCircle(Circle,
												BottomLeftCirclePosition,
												CirclePosition)){
								// Collision 7
								CollisionVector = SubV2(EntityColliderPosition, OtherColliderPosition);
								HitCollider = true;
							} else if(IsPointInCircle(Circle,
												BottomRightCirclePosition,
												CirclePosition)){
								// Collision 8
								CollisionVector = SubV2(EntityColliderPosition, OtherColliderPosition);
								HitCollider = true;
							} else {
								Error("Else branch taken while checking circle-rect collision");
							}
						} else {
							Warning("Undefined collision check between %d and %d", Collider->Type, OtherCollider->Type);
						}

						if(CollisionVector.x != 0.0f || 
							CollisionVector.y != 0.0f){
							AddCollisionEntry(CollisionTable, 
										CollisionVector, Entity, Other);
						}
					}
				}
				Other = Other->Prev;
			}

			if(HitCollider){
				break;
			}
			
			Entity->Transform.Position = Position;
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
				MoveEntity(EntitySentinel, Entity, DeltaTime, 
											&Memory->CollisionTable);
				component_rigid_body * RigidBody = GetComponent(Entity, 
												ComponentType_RigidBody); 
				if(RigidBody){	
					if(Entity->Transform.Position.x < 0.0f){
						if(RigidBody->Velocity.x < 0.0f){
							RigidBody->Velocity.x *= -1.0f;
						}
					} else if (Entity->Transform.Position.x > 12.80f){	
						if(RigidBody->Velocity.x > 0.0){
							RigidBody->Velocity.x *= -1.0f;
						}
					}	
				}
			} break;
			case EntityType_Enemy: {
				component_rigid_body * RigidBody = GetComponent(Entity, 
												ComponentType_RigidBody); 
				if(RigidBody){	
					MoveEntity(EntitySentinel, Entity, DeltaTime,
												&Memory->CollisionTable);
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
