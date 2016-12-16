static void 
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
		InvalidDefaultCase;
	}
	EndStackTraceBlock;
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
