static font *
CreateFont(asset_manager * AssetManager){
	font * Font = Platform.AllocateMemory(sizeof(font));
	ZeroStruct(*Font);

	/*	TODO(furkan)	:
			Font->LettersUpper;
			Font->LettersLower;
	 */

	Font->Numeric[0] = AssetManager->Bitmaps + 
						LoadBitmap(AssetManager, "0");
	Font->Numeric[1] = AssetManager->Bitmaps + 
						LoadBitmap(AssetManager, "1");
	Font->Numeric[2] = AssetManager->Bitmaps + 
						LoadBitmap(AssetManager, "2");
	Font->Numeric[3] = AssetManager->Bitmaps + 
						LoadBitmap(AssetManager, "3");
	Font->Numeric[4] = AssetManager->Bitmaps + 
						LoadBitmap(AssetManager, "4");
	Font->Numeric[5] = AssetManager->Bitmaps + 
						LoadBitmap(AssetManager, "5");
	Font->Numeric[6] = AssetManager->Bitmaps + 
						LoadBitmap(AssetManager, "6");
	Font->Numeric[7] = AssetManager->Bitmaps + 
						LoadBitmap(AssetManager, "7");
	Font->Numeric[8] = AssetManager->Bitmaps + 
						LoadBitmap(AssetManager, "8");
	Font->Numeric[9] = AssetManager->Bitmaps + 
						LoadBitmap(AssetManager, "9");
}
