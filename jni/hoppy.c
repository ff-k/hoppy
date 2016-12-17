#include "hoppy.h"

MEngineInit(GameInit){
	BeginStackTraceBlock;
	Verbose("Initialising the game");
	asset_manager * AssetManager = Memory->AssetManager;
	Verbose("AssetManager is retrieved from game memory");

	entity * Background = CreateEntity(	Memory,
										EntityType_Background,
										V2(12.80f, 7.20f), 
										V2(6.40f, 3.60f));

	entity * Player = CreateEntity(	Memory, 
									EntityType_Player,
									V2(0.68f, 1.28f), V2(6.40f, 6.40f));

	entity * Enemy = CreateEntity(	Memory, EntityType_Enemy,
									V2(1.09f, 1.28f), V2(-1.20f, 5.30f));

	EndStackTraceBlock;
}

MEngineUpdate(GameUpdate){	
	BeginStackTraceBlock;
	ProcessInputs(Memory, Input);
	UpdateEntities(Memory, Input->DeltaTime);
	EndStackTraceBlock;
}
