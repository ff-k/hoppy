#define CollisionTableCapacity 32

typedef struct collision_entry{
	entity * EntityA;
	entity * EntityB;
	v2 CollisionVector;
} collision_entry;

typedef struct collision_table{
	u32 EntryCount;
	collision_entry Entries[CollisionTableCapacity];
} collision_table;
