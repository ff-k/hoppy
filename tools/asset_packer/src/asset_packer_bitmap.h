typedef struct bitmap_header {
	uint16_t Signature;
	uint32_t FileSize;
	uint32_t Reserved;
	uint32_t DataOffset;
} bitmap_header;

typedef struct bitmap_info_header {
	uint32_t Size;
	uint32_t Width;
	uint32_t Height;
	uint16_t Planes;
	uint16_t BitCount;
	uint32_t Compression;
	uint32_t ImageSize;
	uint32_t PixelsPerMeterX;
	uint32_t PixelsPerMeterY;
	uint32_t ColorsUsed;
	uint32_t ColorsImportant;
} bitmap_info_header;

static int 
ReadBitmapHeader(bitmap_header * Header, bitmap_info_header * InfoHeader, FILE * BitmapFile){
	int Success = 1;
	if(fread(&Header->Signature, sizeof(uint16_t), 1, BitmapFile) != 1){
		printf("An error occured while reading signature");
		Success = 0;
	}
	if(fread(&Header->FileSize, sizeof(uint32_t), 1, BitmapFile) != 1){
		printf("An error occured while reading filesize");
		Success = 0;
	}
	if(fread(&Header->Reserved, sizeof(uint32_t), 1, BitmapFile) != 1){
		printf("An error occured while reading reserved bytes");
		Success = 0;
	}
	if(fread(&Header->DataOffset, sizeof(uint32_t), 1, BitmapFile) != 1){
		printf("An error occured while reading data offset");
		Success = 0;
	}
	
	/* Read Info Header */
	if(fread(&InfoHeader->Size           , sizeof(uint32_t), 1, BitmapFile) != 1){
		printf("An error occured while reading info header size");
		Success = 0;
	}
	if(fread(&InfoHeader->Width          , sizeof(uint32_t), 1, BitmapFile) != 1){
		printf("An error occured while reading width");
		Success = 0;
	}
	if(fread(&InfoHeader->Height         , sizeof(uint32_t), 1, BitmapFile) != 1){
		printf("An error occured while reading height");
		Success = 0;
	}
	if(fread(&InfoHeader->Planes         , sizeof(uint16_t), 1, BitmapFile) != 1){
		printf("An error occured while reading planes");
		Success = 0;
	}
	if(fread(&InfoHeader->BitCount       , sizeof(uint16_t), 1, BitmapFile) != 1){
		printf("An error occured while reading bit count");
		Success = 0;
	}
	if(fread(&InfoHeader->Compression    , sizeof(uint32_t), 1, BitmapFile) != 1){
		printf("An error occured while reading compression");
		Success = 0;
	}
	if(fread(&InfoHeader->ImageSize      , sizeof(uint32_t), 1, BitmapFile) != 1){
		printf("An error occured while reading image size");
		Success = 0;
	}
	if(fread(&InfoHeader->PixelsPerMeterX, sizeof(uint32_t), 1, BitmapFile) != 1){
		printf("An error occured while reading pixels per meter x");
		Success = 0;
	}
	if(fread(&InfoHeader->PixelsPerMeterY, sizeof(uint32_t), 1, BitmapFile) != 1){
		printf("An error occured while reading pixels per meter y");
		Success = 0;
	}
	if(fread(&InfoHeader->ColorsUsed     , sizeof(uint32_t), 1, BitmapFile) != 1){
		printf("An error occured while reading colors used");
		Success = 0;
	}
	if(fread(&InfoHeader->ColorsImportant, sizeof(uint32_t), 1, BitmapFile) != 1){
		printf("An error occured while reading colors important");
		Success = 0;
	}

	/*
		TODO(furkan) : Assertions about bitmap format
	*/
	
	return Success;
}