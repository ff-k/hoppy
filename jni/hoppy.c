#include "hoppy.h"

#include "hoppy_spawner.c"

MEngineInit(GameInit){
	BeginStackTraceBlock;

	Memory->CurrentScreen = Screen_MainMenu;

	u32 ScreenIndex;
	for(ScreenIndex=0;
		ScreenIndex<Screen_Count;
		ScreenIndex++){
		game_screen * Screen = Memory->Screens + ScreenIndex;
		Screen->Type = ScreenIndex;

		ui_controller * UI = &Screen->UIController;
		UI->AssetManager = Memory->AssetManager;
		UI->ElementCount = 0;
	}
	
	ui_controller * UI = 0;
	/* MainMenu UIElements */
	UI = &((Memory->Screens + Screen_MainMenu)->UIController);
	rect MenuBackgroundRect;
	MenuBackgroundRect.Size = V2(12.80f, 7.20f);
	v2 MenuBackgroundPosition = V2(6.40f, 3.60f);
	CreateUIElement(UI, MenuBackgroundRect, 
						MenuBackgroundPosition, UIElement_MenuBackground);

	rect GameLogoRect;
	GameLogoRect.Size = V2(8.65f, 3.56f);
	v2 GameLogoPosition = V2(6.40f, 4.50f);
	CreateUIElement(UI, GameLogoRect, 
						GameLogoPosition, UIElement_GameLogo);

	rect PlayButtonRect;
	PlayButtonRect.Size = V2(7.12f, 1.32f);
	v2 PlayButtonPosition = V2(6.40f, 1.80f);
	CreateUIElement(UI, PlayButtonRect, 
						PlayButtonPosition,	UIElement_PlayButton);

	/* InGame UI Elements */
	UI = &((Memory->Screens + Screen_InGame)->UIController);
	rect ScoreBackgroundRect;
	ScoreBackgroundRect.Size = V2(3.24f, 3.24f);
	v2 ScoreBackgroundPosition = V2(0.39f, 6.88f);
	CreateUIElement(UI, ScoreBackgroundRect, 
						ScoreBackgroundPosition, UIElement_BubbleBig);

	rect IconHomeBackgroundRect;
	IconHomeBackgroundRect.Size = V2(3.24f, 3.24f);
	v2 IconHomeBackgroundPosition = V2(13.06f, 7.04f);
	CreateUIElement(UI, IconHomeBackgroundRect, 
						IconHomeBackgroundPosition, UIElement_BubbleBig);

	rect IconHomeRect;
	IconHomeRect.Size = V2(0.64f, 0.64f);
	v2 IconHomePosition = V2(12.24f, 6.60f);
	CreateUIElement(UI, IconHomeRect, 
						IconHomePosition, UIElement_IconHome);

	rect HealthBackgroundRect;
	HealthBackgroundRect.Size = V2(9.60f, 0.64f);
	v2 HealthBackgroundPosition = V2(6.14f, 6.56f);
	CreateUIElement(UI, HealthBackgroundRect, 
					HealthBackgroundPosition, UIElement_SolidRed);

	rect HealthFillRect;
	HealthFillRect.Size = V2(8.96f, 0.48f);
	v2 HealthFillPosition = V2(6.40f, 6.56f);
	CreateUIElement(UI, HealthFillRect, 
					HealthFillPosition, UIElement_SolidYellow);

	Memory->CurrentScreen = Memory->Screens + Screen_MainMenu;
	
	/* EndOfGame UIElements */
	UI = &((Memory->Screens + Screen_EndOfGame)->UIController);
	rect PopUpBackgroundRect;
	PopUpBackgroundRect.Size = V2(8.00f, 4.80f);
	v2 PopUpBackgroundPosition = V2(6.40f, 3.60f);
	CreateUIElement(UI, PopUpBackgroundRect, 
						PopUpBackgroundPosition, UIElement_SolidYellow);

	rect PopUpReplayBackgroundRect;
	PopUpReplayBackgroundRect.Size = V2(1.76f, 1.76f);
	v2 PopUpReplayBackgroundPosition = V2(4.80f, 2.40f);
	CreateUIElement(UI, PopUpReplayBackgroundRect, 
					PopUpReplayBackgroundPosition, UIElement_BubbleSmall);

	rect PopUpReplayIconRect;
	PopUpReplayIconRect.Size = V2(0.96f, 0.96f);
	v2 PopUpReplayIconPosition = V2(4.80f, 2.40f);
	CreateUIElement(UI, PopUpReplayIconRect, 
						PopUpReplayIconPosition, UIElement_IconReplay);

	rect PopUpHomeBackgroundRect;
	PopUpHomeBackgroundRect.Size = V2(1.76f, 1.76f);
	v2 PopUpHomeBackgroundPosition = V2(8.00f, 2.40f);
	CreateUIElement(UI, PopUpHomeBackgroundRect, 
					PopUpHomeBackgroundPosition, UIElement_BubbleSmall);

	rect PopUpHomeIconRect;
	PopUpHomeIconRect.Size = V2(0.96f, 0.96f);
	v2 PopUpHomeIconPosition = V2(8.00f, 2.40f);
	CreateUIElement(UI, PopUpHomeIconRect, 
						PopUpHomeIconPosition, UIElement_IconHome);

	Memory->Font = CreateFont(Memory->AssetManager);

	EndStackTraceBlock;
}

static void
BeginGame(game_memory * Memory){
	BeginStackTraceBlock;

	if(!Memory->Spawners){
		memsz SpawnerSize = sizeof(spawner) * MaxSpawnerCount;
		Memory->Spawners = Platform.AllocateMemory(SpawnerSize);
		ZeroMemory(Memory->Spawners, SpawnerSize);

		spawner_params_enemy EnemySpawnerParams;
		EnemySpawnerParams.Size = V2(0.64f, 0.64f);
		EnemySpawnerParams.SpawnPositionMin = V2(-1.2f, 1.28f);
		EnemySpawnerParams.SpawnPositionMax = V2(-1.2f,
													EnemyPeakPosition);
	
		EnemySpawnerParams.HorizontalVelocityMin = 
												EnemySpeedHorizontalMin;
		EnemySpawnerParams.HorizontalVelocityMax = 
												EnemySpeedHorizontalMax;
	
		EnemySpawnerParams.PeakPositionY = EnemyPeakPosition;
	
		CreateSpawner(Memory->Spawners, &Memory->SpawnerCount,
							EnemySpawnPeriodMin, EnemySpawnPeriodMax,
							SpawnerType_Enemy, &EnemySpawnerParams);
	
		// TODO(furkan) : Global screen dimensions
		r32 ScreenRight = 12.80f;
	
		EnemySpawnerParams.SpawnPositionMin = V2(ScreenRight + 1.2f, 
													1.28f);
		EnemySpawnerParams.SpawnPositionMax = V2(ScreenRight + 1.2f, 
													EnemyPeakPosition);
	
		EnemySpawnerParams.HorizontalVelocityMin = 
												-EnemySpeedHorizontalMin;
		EnemySpawnerParams.HorizontalVelocityMax = 
												-EnemySpeedHorizontalMax;
	
		CreateSpawner(Memory->Spawners, &Memory->SpawnerCount,
							EnemySpawnPeriodMin, EnemySpawnPeriodMax,
							SpawnerType_Enemy, &EnemySpawnerParams);
	} else {
		spawner * Spawners = Memory->Spawners;
		u32 SpawnerCount = Memory->SpawnerCount;
		u32 SpawnerIndex;
		for(SpawnerIndex=0;
			SpawnerIndex<SpawnerCount;
			SpawnerIndex++){
			spawner * Spawner = Spawners + SpawnerIndex;
			Spawner->Counter = 0.0f;
		}
	}

	ClearEntities(&Memory->EntitySentinel);

	entity * Background = CreateEntity(Memory,
										EntityType_Background,
										V2(12.80f, 7.20f), 
										V2(6.40f, 3.60f));

	entity * Player = CreateEntity(Memory, 
									EntityType_Player,
									V2(0.64f, 0.64f), V2(6.40f, 5.30f));

	Memory->CurrentScreen = Memory->Screens + Screen_InGame;

	Memory->Score = 0;

	EndStackTraceBlock;
}

static void
EndGame(game_memory * Memory){
	Memory->CurrentScreen = Memory->Screens + Screen_EndOfGame;
}

MEngineUpdate(GameUpdate){	
	BeginStackTraceBlock;
	
	ui_controller * UI = &Memory->CurrentScreen->UIController;

	switch(Memory->CurrentScreen->Type){
		case Screen_MainMenu:{
			ui_element * PlayButton = &UI->Elements[2];
			if(Input->PointerCount){
				v2 TouchPoint = 
					V2(Input->PointerCoordinates[0].x/PixelsPerUnit,
					7.20f - Input->PointerCoordinates[0].y/PixelsPerUnit);

				/* TODO(furkan) : Set an OnClick function ? */
				if(IsPointInRect(PlayButton->Rect, PlayButton->Position,
															TouchPoint)){
					PlayButton->State = UIElementState_Clicked;
				} else {
					PlayButton->State = UIElementState_Normal;
				}
			} else {
				if(PlayButton->State == UIElementState_Clicked){
					PlayButton->State = UIElementState_Normal;
					BeginGame(Memory);
				}
			}
		} break;
		case Screen_InGame:{
			Memory->Score += (u32) (Input->DeltaTime * 100.0f);
			Memory->CollisionTable.EntryCount = 0;

			ProcessInputs(Memory, Input);
			UpdateEntities(Memory, Input->DeltaTime);
			UpdateSpawners(Memory, Input->DeltaTime);
			HandleCollisions(Memory, Input->DeltaTime);
	
			entity * Player = GetEntity(&Memory->EntitySentinel, 
											EntityType_Player);
	
			component_health * Health = (component_health *)GetComponent(Player, ComponentType_Health);
			r32 HealthFillRate = Health->Current / Health->Max;
	
			ui_element * HealthFill = &UI->Elements[4];
			HealthFill->Rect.Size = V2(HealthFillRate * 8.96f, 
										0.48f);
			HealthFill->Position = AddV2(V2(1.92f, -0.64f),
								V2(HealthFillRate * 4.48f, 7.20f));
	
			ui_element * HomeButton = &UI->Elements[2];
			if(Input->PointerCount){
				v2 TouchPoint = 
				   V2(Input->PointerCoordinates[0].x/PixelsPerUnit,
					7.20f - Input->PointerCoordinates[0].y/PixelsPerUnit);
	
				/* TODO(furkan) : Set an OnClick function ? */
				if(IsPointInRect(HomeButton->Rect, 
									HomeButton->Position,
									TouchPoint)){
					HomeButton->State = UIElementState_Clicked;
				} else {
					HomeButton->State = UIElementState_Normal;
				}
			} else {
				if(HomeButton->State == UIElementState_Clicked){
					HomeButton->State = UIElementState_Normal;
					Memory->CurrentScreen = Memory->Screens + 
												Screen_MainMenu;
				}
			}
		} break;
		case Screen_EndOfGame:{
			ui_element * HomeButton = &UI->Elements[4];
			if(Input->PointerCount){
				v2 TouchPoint = 
				   V2(Input->PointerCoordinates[0].x/PixelsPerUnit,
					7.20f - Input->PointerCoordinates[0].y/PixelsPerUnit);
	
				/* TODO(furkan) : Set an OnClick function ? */
				if(IsPointInRect(HomeButton->Rect, 
									HomeButton->Position,
									TouchPoint)){
					HomeButton->State = UIElementState_Clicked;
				} else {
					HomeButton->State = UIElementState_Normal;
				}
			} else {
				if(HomeButton->State == UIElementState_Clicked){
					HomeButton->State = UIElementState_Normal;
					Memory->CurrentScreen = Memory->Screens + 
												Screen_MainMenu;
				}
			}

			ui_element * ReplayButton = &UI->Elements[2];
			if(Input->PointerCount){
				v2 TouchPoint = 
				   V2(Input->PointerCoordinates[0].x/PixelsPerUnit,
					7.20f - Input->PointerCoordinates[0].y/PixelsPerUnit);
	
				/* TODO(furkan) : Set an OnClick function ? */
				if(IsPointInRect(ReplayButton->Rect, 
									ReplayButton->Position,
									TouchPoint)){
					ReplayButton->State = UIElementState_Clicked;
				} else {
					ReplayButton->State = UIElementState_Normal;
				}
			} else {
				if(ReplayButton->State == UIElementState_Clicked){
					ReplayButton->State = UIElementState_Normal;
					BeginGame(Memory);
				}
			}
		} break;
		InvalidDefaultCase;
	}

	EndStackTraceBlock;
}
