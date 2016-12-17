typedef enum{
	ComponentType_None,
	ComponentType_RigidBody
} component_type;

typedef struct component {
	union {
		u32 ComponentCount;
		void * Structure;
	};

	component_type Type;
	struct component * Next;
} component;

typedef struct component_rigid_body {
	v2 Velocity;
	v2 Acceleration;
	v2 Drag;
} component_rigid_body;

