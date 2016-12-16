#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "asset_packer_bitmap.h"

#define Assert(expr) if(!(expr)) { *(int *)0 = 0; }
#define ZeroStruct(str) memset(&(str), 0, sizeof(str))

#define AssetPackageFileFormat (('M'<<24) | ('A'<<16) | ('S'<< 8) | ('S'))
#define AssetPackageVersion 0x0001

#define FileNameMaxSize FILENAME_MAX
#define AssetIDMaxSize 32

typedef enum {
	ParserState_None,
	ParseState_OutputFile,
	ParserState_Bitmap
} parser_state;

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

typedef struct program_state {
	char OutputFileName[FileNameMaxSize];

	uint16_t AssetTableCapacity;
	uint16_t AssetTypeCapacity;
	uint32_t AssetBitmapCapacity;
	uint32_t BitmapCapacity;
	
	uint32_t BitmapFileCount;
	char ** BitmapFileNames;
	char ** BitmapIDs;
} program_state;

static void 
ShowUsage(char * ExecutableName){
	printf("Usage :\n\t%s -b <bitmap_file_1> ... <bitmap_file_n>\n", ExecutableName);
}

static int 
FindCharInString(char Char, char * String, int StringLength) {
	/* NOTE(furkan) : Returns StringLength on fail */
	int CharIndex;
	for (CharIndex = 0; CharIndex < StringLength; CharIndex++) {
		if (String[CharIndex] == Char) {
			break;
		}
	}
	return CharIndex;
}