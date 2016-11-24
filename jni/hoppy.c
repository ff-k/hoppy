#include "hoppy.h"

#define MaxDistanceFromGround 5.12f

u32 BitmapIndex = -1;
MEngineInit(GameInit){
	Verbose("Initialising the game");
	asset_manager * AssetManager = Memory->AssetManager;
	Verbose("AssetManager is retrieved from game memory");
	BitmapIndex = AssetManagerLoadBitmap(&Memory->PlatformAPI, AssetManager);
	Verbose("Loaded bitmap with asset manager");
	Assert(BitmapIndex != -1);
}

v2 LastPosition = {2.56f, 2.56f};

MEngineUpdate(GameUpdate){
	render_command_entry_clear TestClear;
	TestClear.Color.r = 1.0f;
	TestClear.Color.g = 0.0f;
	TestClear.Color.b = 1.0f;

	PushRenderCommand(RenderCommands, 
					  RenderCommandEntryType_Clear, 
					  &TestClear);	

	render_command_entry_drawrect TestDrawRect;
	TestDrawRect.Rect.Size = V2(12.8f, 0.64f);
	TestDrawRect.Rect.Position = V2(Memory->Screen.x/200.0f, 
									0.32f);
	TestDrawRect.Color = V4(0.0f, 1.0f, 0.0f, 1.0f);

	PushRenderCommand(RenderCommands, 
					  RenderCommandEntryType_DrawRect, 
					  &TestDrawRect);

	static r32 SecondRectPosition = 0.0f;
	TestDrawRect.Rect.Size = V2(7.2f, 7.2f);
	TestDrawRect.Rect.Position = V2(Memory->Screen.x/200.0f+6.4f*sinf(SecondRectPosition)*Input->DeltaTime*0.01f, Memory->Screen.y/200.0f);
	TestDrawRect.Color = V4(0.0f, 0.0f, 1.0f, 1.0f);

	PushRenderCommand(RenderCommands, 
					  RenderCommandEntryType_DrawRect, 
					  &TestDrawRect);
	SecondRectPosition += 0.1f;

	r32 Width = 0.32f;
	r32 Height = 0.32f;
	r32 Speed = 0.00025f;
	static r32 DistanceFactor = 0.0f;
	r32 DistanceFromGround = sinf(DistanceFactor) * MaxDistanceFromGround;
	DistanceFactor += Speed*Input->DeltaTime;
	LastPosition.y = DistanceFromGround;
	if(DistanceFromGround<0.0f){
		LastPosition.y *= -1.0f;
	}
	LastPosition.y += 0.8f;
	v2 Screen = Memory->Screen;
	if(Input->PointerCount){
	
		if(Input->PointerCoordinates[0].x > Screen.x/2.0f){
			LastPosition.x += 0.1f;
		} else {
			LastPosition.x -= 0.1f;
		}
	}
#define MovementLimitLeft (Width/2.0f)
#define MovementLimitRight (Screen.x/100.0f-Width/2.0f)

	if(LastPosition.x < MovementLimitLeft){
		LastPosition.x = MovementLimitLeft;
	} else if(LastPosition.x > MovementLimitRight){
		LastPosition.x = MovementLimitRight;
	}

	render_command_entry_drawbitmap TestDrawBitmap;
	TestDrawBitmap.Position = LastPosition;
	TestDrawBitmap.Size = V2(Width, Height);
	TestDrawBitmap.Bitmap = Memory->AssetManager->Bitmaps + BitmapIndex;

	PushRenderCommand(RenderCommands,
					  RenderCommandEntryType_DrawBitmap,
					  &TestDrawBitmap);
}
