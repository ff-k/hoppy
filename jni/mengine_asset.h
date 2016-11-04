#define AssetManagerBitmapCapacity 64

typedef struct asset_bitmap{
	u32 * Data;
	u32 Width;
	u32 Height;
} asset_bitmap;

typedef struct asset_manager{
	asset_bitmap Bitmaps[AssetManagerBitmapCapacity];
	u32 BitmapCount;
} asset_manager;