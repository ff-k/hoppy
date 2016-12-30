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
		case ComponentType_Health: {
			component_health * Health = Platform.AllocateMemory(sizeof(component_health));
			ZeroStruct(*Health);
			Component->Structure = Health;
		} break;
		case ComponentType_Attack: {
			component_attack * Attack = Platform.AllocateMemory(sizeof(component_attack));
			ZeroStruct(*Attack);
			Component->Structure = Attack;
		} break;
		InvalidDefaultCase;
	}

	EndStackTraceBlock;
	return Component;
}

static void
ClearComponents(component * ComponentSentinel){
	component * Component = ComponentSentinel->Next;
	while(Component != ComponentSentinel){
		component * NextComponent = Component->Next;
	
		free(Component->Structure);
		free(Component);

		Component = NextComponent;
	}

	ComponentSentinel->Next = ComponentSentinel;
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

	return Collider;
}

static component_health *
AddHealthComponent(entity * Entity, r32 HealthMax){
	component * Component = AddComponent(Entity, ComponentType_Health);
	component_health * Health = Component->Structure;
	Health->Current = Health->Max = HealthMax;

	return Health;
}

static void * GetComponent(entity *, component_type);

static component_attack *
AddAttackComponent(entity * Entity, r32 HitPoint){
	component * Component = AddComponent(Entity, ComponentType_Attack);
	component_attack * Attack = Component->Structure;
	Attack->HitPoint = HitPoint;

	return Attack;
}

static void * 
GetComponent(entity * Entity, component_type Type){
	BeginStackTraceBlock;
	void * Result = 0;
	component * Component = Entity->ComponentSentinel.Next;
	b32 Verbose = Type == ComponentType_Attack;
//	if(Verbose) Warning("Searching : %d", Type);
	while(Component != &Entity->ComponentSentinel){
//		if(Verbose) Warning("ComponentType : %d", Component->Type);
		if(Component->Type == Type){
			Result = Component->Structure;
			break;
		}

		Component = Component->Next;
	}
	EndStackTraceBlock;
	return Result;
}
