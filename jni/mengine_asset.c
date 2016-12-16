static u32
LoadBitmap(asset_manager * Manager, char * BitmapID){
	BeginStackTraceBlock;
	u32 BitmapIndex = 0;

	/*	TODO(furkan) : Currently, asset manager reads all assets
		at the beginning and stores it in memory whole game time.
		Obviously, that is not feasible for games that have large
		amounts of assets.
		Create an asset cache and if bitmap is found in cache, 
		load it from cache. Otherwise, read it from asset file.
	*/

	/* NOTE(furkan) : Bitmap color format that OpenGL 
		expects is 0xAABBGGRR.
		Also, values below are premultiplied. Do not forget to multiply
		R,G,B with A value while importing bitmaps 	
	*/
	if(Manager->Bitmaps){
		asset_type * AssetTypeBitmap = 0;
		u16 AssetTypeCount = Manager->AssetTypeCount;
		u16 AssetTypeIndex;
		for(AssetTypeIndex=0; 
			AssetTypeIndex<AssetTypeCount; 
			AssetTypeIndex++){
			
			asset_type * AssetType = Manager->AssetTypes + AssetTypeIndex;
			if(!strncmp(AssetType->ID, "Bitmap", sizeof(AssetType->ID))){
				AssetTypeBitmap = AssetType;
				break;
			}
		}

		if(AssetTypeBitmap){
			u32 BitmapCount = AssetTypeBitmap->AssetCount;
			for(BitmapIndex=0; BitmapIndex<BitmapCount; BitmapIndex++){
				asset_table_entry * Entry = AssetTypeBitmap->AssetTable + BitmapIndex;
				if(!strncmp(Entry->ID, BitmapID, sizeof(Entry->ID))){
					break;
				}
			}
			if(BitmapIndex == BitmapCount){
				/* Not found */
				BitmapIndex = 0;
			}
		}
	}	
	
	EndStackTraceBlock;
	return BitmapIndex;
}
