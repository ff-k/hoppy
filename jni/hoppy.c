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
									V2(0.68f, 1.28f), V2(2.56f, 2.56f));

	entity * Enemy = CreateEntity(	Memory, EntityType_Enemy,
									V2(1.09f, 1.28f), V2(3.60f, 3.60f));

	EndStackTraceBlock;
}

MEngineUpdate(GameUpdate){	
	BeginStackTraceBlock;
	UpdateEntities(Memory, Input);
	EndStackTraceBlock;
}
