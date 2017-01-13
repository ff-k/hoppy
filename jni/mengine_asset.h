#define AssetManagerBitmapCapacity 64

#define AssetPackageFileFormat (('M'<<24) | ('A'<<16) | ('S'<< 8) | ('S'))
#define AssetIDMaxSize 32

typedef struct asset_header {
	uint32_t FileFormat;
	uint16_t Version;
} asset_header;

typedef struct asset_table_entry {
	uint8_t ID[AssetIDMaxSize];
	uint32_t Offset;
} asset_table_entry;

typedef struct asset_type {
	uint8_t ID[8];
	uint32_t AssetCount;
	asset_table_entry *  AssetTable;
} asset_type;

typedef struct asset_bitmap {
	uint32_t Width;
	uint32_t Height;
	uint32_t * Pixels;
} asset_bitmap;

typedef struct asset_package {
	asset_header Header;
	
	uint16_t AssetTypeCount;
	asset_type * AssetTypes;
	
	asset_bitmap * Bitmaps;
} asset_package;

typedef struct asset_file {
	char * Path;

	s32 Descriptor;
	s32 Offset;
	s32 Length;
} asset_file;
/* NOTE(furkan) : Should be implemented on platform layer */
static b32 OpenFileDescriptor(asset_file * File);	

typedef struct asset_manager{
	uint16_t AssetTypeCount;
	asset_type * AssetTypes;

	asset_bitmap * Bitmaps;
} asset_manager;
