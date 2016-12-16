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
	v3 Velocity;
	v3 Acceleration;
} component_rigid_body;

