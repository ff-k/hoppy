typedef enum{
	Screen_MainMenu,
	Screen_InGame,
	Screen_EndOfGame,

	Screen_Count
} game_screen_type;

typedef struct game_screen {
	ui_controller UIController;
	game_screen_type Type;
} game_screen;
