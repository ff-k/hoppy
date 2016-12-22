typedef struct transform {
	v2 Position;
	v2 Rotation;
	v2 Scale;
} transform;

typedef enum {
	EntityType_None,
	EntityType_Background,
	EntityType_Player,
	EntityType_Enemy
} entity_type;

typedef struct entity {	
	v2 Dimension;
	transform Transform;
	entity_type Type;

	b32 Collided;
	b8 IsEnabled;
	b8 IsVisible;
	struct entity * SubEntity;
	struct entity * Next;
	struct entity * Prev;

	u32 BitmapIndex;

	component ComponentSentinel;
} entity;
