#include "asset_packer.h"

/* TODO(furkan) : 
	AssetID collision check
	Assertions in ReadBitmapHeader
*/

static asset_type * 
AddAssetType(asset_package * AssetPackage, uint16_t AssetCapacity, uint16_t AssetTypeCapacity, char * AssetTypeID){
	Assert(AssetPackage->AssetTypeCount < AssetTypeCapacity);
	
	asset_type * AssetType = AssetPackage->AssetTypes + AssetPackage->AssetTypeCount;
	strncpy((char *)AssetType->ID, AssetTypeID, sizeof(AssetType->ID));
	
	AssetType->AssetTable = (asset_table_entry *)malloc(sizeof(asset_table_entry) * AssetCapacity);
	memset(AssetType->AssetTable, 0, sizeof(asset_table_entry) * AssetCapacity);

	AssetPackage->AssetTypeCount++;
	
	return AssetType;
}

static void 
AddBitmapAssets(	asset_package * AssetPackage, 
					asset_type * AssetTypeBitmap, uint32_t AssetBitmapCapacity, 
					char ** BitmapFileNames, char ** BitmapIDs, uint32_t BitmapFileCount){
	int BitmapIndex;
	for(BitmapIndex=0; BitmapIndex<BitmapFileCount; BitmapIndex++){
		bitmap_header Header;
		bitmap_info_header InfoHeader;
		
		FILE * BitmapFile = 0;
		char * BitmapFileName = BitmapFileNames[BitmapIndex];
		BitmapFile = fopen(BitmapFileName, "rb");
		if(BitmapFile){
			/* Read Header */
			if(ReadBitmapHeader(&Header, &InfoHeader, BitmapFile)){
				uint32_t PixelCount = InfoHeader.Width*InfoHeader.Height;
				uint32_t ByteCount = InfoHeader.BitCount / 8;
				size_t PixelDataSize = PixelCount*ByteCount;
				uint32_t * Pixels = (uint32_t *)malloc(PixelDataSize);
				if (fread(Pixels, ByteCount, PixelCount, BitmapFile) != PixelCount) {
					printf("An error occured while reading pixels for %s\n", BitmapFileName);
				}
				
				/*	NOTE(furkan) : Layout of the data read from bitmaps :
						Pixel Order :	4 5 6 7
										0 1 2 3
		
						Byte Order :	0xAARRGGBB
				*/
							
				uint32_t PixelIndex;
				for(PixelIndex=0; PixelIndex<PixelCount; PixelIndex++){
					uint32_t RedMask  = (Pixels[PixelIndex] & 0x00FF0000) >> 16;
					uint32_t BlueMask = (Pixels[PixelIndex] & 0x000000FF) << 16;
					
					Pixels[PixelIndex] = (Pixels[PixelIndex] & 0xFF00FF00) | BlueMask | RedMask;
				}
				
				Assert(AssetTypeBitmap->AssetCount < AssetBitmapCapacity);
				asset_bitmap * AssetBitmap = AssetPackage->Bitmaps + AssetTypeBitmap->AssetCount;
				AssetBitmap->Width = InfoHeader.Width;
				AssetBitmap->Height = InfoHeader.Height;
				AssetBitmap->Pixels = Pixels;
				
				asset_table_entry * AssetEntry = AssetTypeBitmap->AssetTable + AssetTypeBitmap->AssetCount;
				strncpy((char *)AssetEntry->ID, BitmapIDs[BitmapIndex], sizeof(AssetEntry->ID));
	
				AssetTypeBitmap->AssetCount++;

				fclose(BitmapFile);
			} else {
				printf("Header could not been read for %s\n", BitmapFileName);
			}
		} else {
			printf("%s could not be read, ignored\n", BitmapFileName);
		}
	}
}

static void 
InitialiseAssetPackage(asset_package * AssetPackage, uint16_t AssetTypeCapacity){
	AssetPackage->Header.FileFormat = AssetPackageFileFormat;
	AssetPackage->Header.Version = AssetPackageVersion;
	
	AssetPackage->AssetTypes = (asset_type *)malloc(sizeof(asset_type) * AssetTypeCapacity);
	memset(AssetPackage->AssetTypes, 0, sizeof(asset_type) * AssetTypeCapacity);
}

static int 
InitialiseProgram(program_state * State, int ArgCount, char ** Args){
	int Success = 1;

	State->AssetTableCapacity = 512;
	State->AssetTypeCapacity = 8;
	State->AssetBitmapCapacity = 16;
	State->BitmapCapacity = 32;
	
	State->BitmapFileNames = (char **) malloc(sizeof(char *) * State->BitmapCapacity);
	State->BitmapIDs = (char **)malloc(sizeof(char *) * State->BitmapCapacity);

	char * DefaultFileNamePrefix = "asset_packer_out_";
	size_t PrefixLength = sizeof(char) * 17;
	srand(time(0));
	int RandomStamp = rand() % 100000000;
	char RandomStampStr[8];
	char * DefaultFileNamePostfix = ".mass";
	size_t PostfixLength = sizeof(char) * 5;
	int RandomStampLength = snprintf(RandomStampStr, sizeof(RandomStampStr), "%d", RandomStamp);

	Assert(PrefixLength + sizeof(RandomStampStr) + PostfixLength <= sizeof(State->OutputFileName));
	strncpy(State->OutputFileName, DefaultFileNamePrefix, PrefixLength);
	strncpy(State->OutputFileName + PrefixLength, RandomStampStr, sizeof(RandomStampStr));
	strncpy(State->OutputFileName + PrefixLength + RandomStampLength, DefaultFileNamePostfix, PostfixLength);

	uint32_t BitmapIndex;
	for(BitmapIndex=0; BitmapIndex<State->BitmapCapacity; BitmapIndex++){
		State->BitmapFileNames[BitmapIndex] = (char *)malloc(sizeof(char) * FileNameMaxSize);
		memset(State->BitmapFileNames[BitmapIndex], 0, sizeof(char) * FileNameMaxSize);

		State->BitmapIDs[BitmapIndex] = (char *)malloc(sizeof(char) * AssetIDMaxSize);
		memset(State->BitmapIDs[BitmapIndex], 0, sizeof(char) * AssetIDMaxSize);
	}
	
	parser_state ParserState = ParserState_None;
	int ArgIndex;
	for (ArgIndex = 1; ArgIndex < ArgCount; ArgIndex++) {
		size_t ArgLength = strlen(Args[ArgIndex]);

		if (!strncmp(Args[ArgIndex], "-b", ArgLength) ||
			!strncmp(Args[ArgIndex], "--bitmap", ArgLength)) {
			ParserState = ParserState_Bitmap;
		}
		else if (!strncmp(Args[ArgIndex], "-o", ArgLength) ||
				 !strncmp(Args[ArgIndex], "--output", ArgLength)) {
			ParserState = ParseState_OutputFile;
		} else {
			switch (ParserState)	{
				case ParserState_None: {
					/* NOTE(furkan) : Do nothing for now */
				} break;
				case ParseState_OutputFile: {
					if (ArgLength > FileNameMaxSize) {
						printf("Filename for output cannot be longer than %d characters, terminating... \n", FileNameMaxSize);
						Success = 0;
					} else {
						strncpy(State->OutputFileName, Args[ArgIndex], sizeof(State->OutputFileName));
					}

					ParserState = ParserState_None;
				} break;
				case ParserState_Bitmap: {
					int ColonIndex = FindCharInString(':', Args[ArgIndex], ArgLength);
					int Valid = 1;
					if (ColonIndex > FileNameMaxSize) {
						printf("Bitmap filenames cannot be longer than %d characters. Bitmap will be ignored : %s\n", FileNameMaxSize, Args[ArgIndex]);
						Valid = 0;
					}

					int IDLength = ArgLength - ColonIndex - 1;
					if (IDLength > AssetIDMaxSize) {
						printf("Bitmap IDs cannot be longer than %d characters. Bitmap will be ignored : %s\n", AssetIDMaxSize, Args[ArgIndex]);
						Valid = 0;
					}

					if (ColonIndex == 0) {
						printf("File path is not specified for bitmap and will be ignored : %s\n", Args[ArgIndex]);
						Valid = 0;
					}

					if (ColonIndex >= (ArgLength - 1)) {
						printf("ID is not specified for bitmap and will be ignored : %s\n", Args[ArgIndex]);
						Valid = 0;
					}

					if (Valid) {
						strncpy(State->BitmapFileNames[State->BitmapFileCount], Args[ArgIndex], ColonIndex);
						strncpy(State->BitmapIDs[State->BitmapFileCount], Args[ArgIndex] + (ColonIndex + 1), IDLength);
						State->BitmapFileCount++;
					}
				} break;
				default: {
					printf("Parser state %d is undefined\n", ParserState);
				} break;
			}
		}
	}
	
	return Success;
}

static void 
CalculateOffsetValues(asset_package * AssetPackage) {
	size_t Offset = 0;
	Offset += sizeof(AssetPackage->Header.FileFormat);
	Offset += sizeof(AssetPackage->Header.Version);
	Offset += sizeof(AssetPackage->AssetTypeCount);
	int Index;
	for (Index = 0; Index < AssetPackage->AssetTypeCount; Index++) {
		Offset += sizeof(AssetPackage->AssetTypes[Index].ID);
		Offset += sizeof(AssetPackage->AssetTypes[Index].AssetCount);
		int Index_2;
		for (Index_2 = 0; Index_2 < AssetPackage->AssetTypes[Index].AssetCount; Index_2++) {
			Offset += sizeof(AssetPackage->AssetTypes[Index].AssetTable[Index_2].ID);
			Offset += sizeof(AssetPackage->AssetTypes[Index].AssetTable[Index_2].Offset);
		}
	}

	uint8_t BytesPerPixel = 4;
	asset_type * AssetTypeBitmap = AssetPackage->AssetTypes + 0;
	for (Index = 0; Index < AssetTypeBitmap->AssetCount; Index++) {
		asset_table_entry * Entry = AssetTypeBitmap->AssetTable + Index;
		Entry->Offset = Offset;

		asset_bitmap * Bitmap = AssetPackage->Bitmaps + Index;

		Offset += sizeof(Bitmap->Width);
		Offset += sizeof(Bitmap->Height);
		Offset += Bitmap->Width * Bitmap->Height * BytesPerPixel;
	}
}

static void 
OutputAssetPackage(char * OutputFileName, asset_package * AssetPackage) {
	FILE * Output = fopen(OutputFileName, "wb");
	if (Output) {

		fwrite(&AssetPackage->Header.FileFormat, sizeof(AssetPackage->Header.FileFormat), 1, Output);
		fwrite(&AssetPackage->Header.Version, sizeof(AssetPackage->Header.Version), 1 , Output);
		fwrite(&AssetPackage->AssetTypeCount, sizeof(AssetPackage->AssetTypeCount), 1 , Output);

		int Index;
		for (Index = 0; Index < AssetPackage->AssetTypeCount; Index++) {
			fwrite(&AssetPackage->AssetTypes[Index].ID, sizeof(AssetPackage->AssetTypes[Index].ID), 1 , Output);
			fwrite(&AssetPackage->AssetTypes[Index].AssetCount, sizeof(AssetPackage->AssetTypes[Index].AssetCount), 1, Output);
			int Index_2;
			for (Index_2 = 0; Index_2 < AssetPackage->AssetTypes[Index].AssetCount; Index_2++) {
				fwrite(&AssetPackage->AssetTypes[Index].AssetTable[Index_2].ID, sizeof(AssetPackage->AssetTypes[Index].AssetTable[Index_2].ID), 1, Output);
				fwrite(&AssetPackage->AssetTypes[Index].AssetTable[Index_2].Offset, sizeof(AssetPackage->AssetTypes[Index].AssetTable[Index_2].Offset), 1, Output);
			}
		}

		uint8_t BytesPerPixel = 4;
		asset_type * AssetTypeBitmap = AssetPackage->AssetTypes + 0;
		for (Index = 0; Index < AssetTypeBitmap->AssetCount; Index++) {
			asset_bitmap * Bitmap = AssetPackage->Bitmaps + Index;

			fwrite(&Bitmap->Width, sizeof(Bitmap->Width), 1, Output);
			fwrite(&Bitmap->Height, sizeof(Bitmap->Height), 1, Output);
			fwrite(Bitmap->Pixels, BytesPerPixel, Bitmap->Width * Bitmap->Height, Output);
		}

		fclose(Output);
	} else {
		printf("Output file could not be opened\n");
	}
}

int main(int argc, char ** argv){	
	if (argc < 3) {
		ShowUsage(argv[0]);
		return -1;
	}

	program_state State;
	ZeroStruct(State);
	
	if(InitialiseProgram(&State, argc, argv)){
		asset_package AssetPackage;
		ZeroStruct(AssetPackage);
		InitialiseAssetPackage(&AssetPackage, State.AssetTypeCapacity);

		/* NOTE(furkan) : Add bitmaps */
		asset_type * AssetTypeBitmap = AddAssetType(&AssetPackage, State.AssetTableCapacity, State.AssetTypeCapacity, "Bitmap");

		AssetPackage.Bitmaps = (asset_bitmap *)malloc(sizeof(asset_bitmap) * State.AssetBitmapCapacity);
		memset(AssetPackage.Bitmaps, 0, sizeof(asset_bitmap) * State.AssetBitmapCapacity);

		AddBitmapAssets(&AssetPackage, AssetTypeBitmap, State.AssetBitmapCapacity,
			State.BitmapFileNames, State.BitmapIDs, State.BitmapFileCount);

		CalculateOffsetValues(&AssetPackage);

		OutputAssetPackage(State.OutputFileName, &AssetPackage);
	}
	
	return 0;
}
