static ui_element *
CreateUIElement(ui_controller * UIController,
				rect UIRect, v2 Position, ui_element_type Type){

	Assert(UIController->ElementCount < UIElementCapacity);

	ui_element * Element = UIController->Elements + 
								UIController->ElementCount;
	ZeroStruct(*Element);

	Element->Rect = UIRect;
	Element->Position = Position;
	Element->IsEnabled = true;
	Element->Type = Type;

	asset_manager * AssetManager = UIController->AssetManager;

	switch(Type) {
		case UIElement_MenuBackground:{
			Element->BitmapIndex[UIElementState_Normal] = 
							LoadBitmap(AssetManager, "Background");
		} break;
		case UIElement_GameLogo:{
			Element->BitmapIndex[UIElementState_Normal] = 
							LoadBitmap(AssetManager, "GameLogo");
		} break;
		case UIElement_PlayButton:{
			Element->BitmapIndex[UIElementState_Normal] =
							LoadBitmap(AssetManager, "PlayButtonNormal");
			Element->BitmapIndex[UIElementState_Clicked] =
							LoadBitmap(AssetManager, "PlayButtonClicked");
		} break;
		case UIElement_BubbleBig:{
			Element->BitmapIndex[UIElementState_Normal] = 
							LoadBitmap(AssetManager, "BubbleBig");
		} break;
		case UIElement_BubbleSmall:{
			Element->BitmapIndex[UIElementState_Normal] = 
							LoadBitmap(AssetManager, "BubbleSmall");
		} break;
		case UIElement_IconHome:{
			Element->BitmapIndex[UIElementState_Normal] = 
							LoadBitmap(AssetManager, "IconHome");
			Element->BitmapIndex[UIElementState_Clicked] = 
							LoadBitmap(AssetManager, "IconHome");
		} break;
		case UIElement_IconReplay:{
			Element->BitmapIndex[UIElementState_Normal] = 
							LoadBitmap(AssetManager, "IconReplay");
			Element->BitmapIndex[UIElementState_Clicked] = 
							LoadBitmap(AssetManager, "IconReplay");
		} break;
		case UIElement_SolidRed:{
			Element->BitmapIndex[UIElementState_Normal] = 
							LoadBitmap(AssetManager, "HealthBackground");
		} break;
		case UIElement_SolidYellow:{
			Element->BitmapIndex[UIElementState_Normal] = 
							LoadBitmap(AssetManager, "HealthFill");
		} break;
		InvalidDefaultCase;
	}

	UIController->ElementCount++;

	return Element;
}
