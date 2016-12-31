#define LetterCount 26

typedef struct font{
	asset_bitmap * LettersUpper[LetterCount];
	asset_bitmap * LettersLower[LetterCount];
	asset_bitmap * Numeric[10];
} font;
