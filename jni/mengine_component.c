static component *
AddComponent(entity * Entity, component_type Type){
	BeginStackTraceBlock;
	component * Component = Platform.AllocateMemory(sizeof(component));
	ZeroStruct(*Component);

	Component->Next = Entity->ComponentSentinel.Next;
	Entity->ComponentSentinel.Next = Component;
	Entity->ComponentSentinel.ComponentCount++;

	Component->Type = Type;
	switch(Type){
		case ComponentType_RigidBody:{
			component_rigid_body * RigidBody = Platform.AllocateMemory(sizeof(component_rigid_body));
			ZeroStruct(*RigidBody);
			Component->Structure = RigidBody;
		} break;
		case ComponentType_Collider:{
			component_collider * Collider = Platform.AllocateMemory(sizeof(component_collider));
			ZeroStruct(*Collider);
			Component->Structure = Collider;
		} break;
		InvalidDefaultCase;
	}

	EndStackTraceBlock;
	return Component;
}

static component_collider *
AddCollider(entity * Entity, collider_type ColliderType, void * Bounds){
	component * Component = AddComponent(Entity, ComponentType_Collider);
	component_collider * Collider = Component->Structure;
	Collider->Type = ColliderType;
	switch(ColliderType){
		case ColliderType_Rect:{
			Collider->Size = *((v2 *)Bounds);
		} break;
		case ColliderType_Circle:{
			Collider->Radius = *((r32 *)Bounds);
		} break;
		InvalidDefaultCase;
	}
}

static void * 
GetComponent(entity * Entity, component_type Type){
	BeginStackTraceBlock;
	void * Result = 0;
	component * Component = Entity->ComponentSentinel.Next;
	while(Component != &Entity->ComponentSentinel){
		if(Component->Type == Type){
			Result = Component->Structure;
			break;
		}

		Component = Component->Next;
	}
	EndStackTraceBlock;
	return Result;
}
