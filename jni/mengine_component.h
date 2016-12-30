typedef enum{
	ComponentType_None,
	ComponentType_RigidBody,
	ComponentType_Collider,
	ComponentType_Health,
	ComponentType_Attack
} component_type;

typedef struct component {
	union {
		u32 ComponentCount;
		void * Structure;
	};

	component_type Type;
	struct component * Next;
} component;

typedef struct component_attack {
	r32 HitPoint;
} component_attack;

typedef struct component_health {
	r32 Max;
	r32 Current;
} component_health;

typedef struct component_rigid_body {
	v2 Velocity;
	v2 Acceleration;
	v2 Drag;
} component_rigid_body;

typedef enum{
	ColliderType_None,
	ColliderType_Rect,
	ColliderType_Circle
} collider_type;

typedef struct component_collider{
	collider_type Type;
	v2 Offset;
	union{	
		struct{
			/* Rect collider */
			v2 Size;
		};
		struct{
			/* Circle collider */
			r32 Radius;
		};
	};
} component_collider;
