#include "hoppy.h"

#define MaxDistanceFromGround 512.0f

u32 BitmapIndex = -1;
MEngineInit(GameInit){
	Verbose("Initialising the game");
	asset_manager * AssetManager = Memory->AssetManager;
	Verbose("AssetManager is retrieved from game memory");
	BitmapIndex = AssetManagerLoadBitmap(&Memory->PlatformAPI, AssetManager);
	Verbose("Loaded bitmap with asset manager");
	Assert(BitmapIndex != -1);
}

v2 LastPosition = {256.0f, 256.0f};

MEngineUpdate(GameUpdate){
	render_command_entry_clear TestClear;
	TestClear.Color.r = 0.4f;
	TestClear.Color.g = 0.2f;
	TestClear.Color.b = 0.8f;

	PushRenderCommand(RenderCommands, 
					  RenderCommandEntryType_Clear, 
					  &TestClear);	

	render_command_entry_drawrect TestDrawRect;
	TestDrawRect.Rect.Size.x = 128;
	TestDrawRect.Rect.Size.y = 64;

	TestDrawRect.Rect.Color.r = 0.4f;
	TestDrawRect.Rect.Color.g = 1.0f;
	TestDrawRect.Rect.Color.b = 0.1f;

	TestDrawRect.Rect.Position.x = 32;
	TestDrawRect.Rect.Position.y = 256;

	rect TestRect;
	PushRenderCommand(RenderCommands, 
					  RenderCommandEntryType_DrawRect, 
					  &TestDrawRect);

	r32 Width = 32.0f;
	if(Width < 0.0f) Width *= -1.0f;

	r32 Height = 32.0f;
	if(Height < 0.0f) Height *= -1.0f;

	r32 Speed = 0.25f;
	r32 DistanceFromGround = sinf(LastPosition.y+Input->DeltaTime*Speed) * MaxDistanceFromGround;
	if(DistanceFromGround<Height/2.0f){
		DistanceFromGround *= -1.0f;
	}
	LastPosition.y = DistanceFromGround;

	v4 Position[4];
	Position[0].x = LastPosition.x-Width/2.0f;
	Position[0].y = LastPosition.y+Height/2.0f;
	Position[0].z =  0.0f;
	Position[0].w =  1.0f;

	Position[1].x = LastPosition.x+Width/2.0f;
	Position[1].y = LastPosition.y+Height/2.0f;
	Position[1].z =  0.0f;
	Position[1].w =  1.0f;

	Position[2].x = LastPosition.x+Width/2.0f;
	Position[2].y = LastPosition.y-Height/2.0f;
	Position[2].z =  0.0f;
	Position[2].w =  1.0f;

	Position[3].x = LastPosition.x-Width/2.0f;
	Position[3].y = LastPosition.y-Height/2.0f;
	Position[3].z =  0.0f;
	Position[3].w =  1.0f;

	if(Input->PointerCount){
		v2 Screen = Memory->Screen;
		
		if(Input->PointerCoordinates[0].x > Screen.x/2.0f){
			LastPosition.x += 10.0f;
		} else {
			LastPosition.x -= 10.0f;
		}
	}

	v2 TextureUV[4];
	TextureUV[0].u = 0.0f;
	TextureUV[0].v = 1.0f;

	TextureUV[1].u = 1.0f;
	TextureUV[1].v = 1.0f;

	TextureUV[2].u = 1.0f;
	TextureUV[2].v = 0.0f;

	TextureUV[3].u = 0.0f;
	TextureUV[3].v = 0.0f;

	render_command_entry_drawbitmap TestDrawBitmap;
	memcpy(TestDrawBitmap.Position, Position, sizeof(Position));
	memcpy(TestDrawBitmap.UV, TextureUV, sizeof(TextureUV));
	TestDrawBitmap.Bitmap = Memory->AssetManager->Bitmaps + BitmapIndex;

	PushRenderCommand(RenderCommands,
					  RenderCommandEntryType_DrawBitmap,
					  &TestDrawBitmap);
}
