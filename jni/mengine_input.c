static void
ProcessInputs(game_memory * Memory, game_input * Input){
	BeginStackTraceBlock;

	/*	TODO(furkan) : Store list of entities which 
		will be affected by input seperately to make 
		processing faster
	*/

	entity * EntitySentinel = &Memory->EntitySentinel;
	entity * Entity = EntitySentinel->Prev;

	while(Entity != EntitySentinel){
		switch(Entity->Type){
			case EntityType_Player: {
				component_rigid_body * RigidBody;
				RigidBody = GetComponent(Entity, ComponentType_RigidBody);

				if(Input->PointerCount){
					r32 ScreenHalfWidth = (r32)Memory->ScreenDim.x/2.0f;
					if(Input->PointerCoordinates[0].x > ScreenHalfWidth){
						RigidBody->Acceleration = V2(15.0f, 
													GravityAcceleration);
					} else {
						RigidBody->Acceleration = V2(-15.0f, 
													GravityAcceleration);
					}
				} else {
					RigidBody->Acceleration = V2(0.0f, 
												GravityAcceleration);
				}
			} break;
			case EntityType_Enemy:
			case EntityType_Background : {
			} break;
			InvalidDefaultCase;
		}
		Entity = Entity->Prev;
	}
	EndStackTraceBlock;
}

