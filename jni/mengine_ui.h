typedef enum {
	UIElement_MenuBackground,
	UIElement_GameLogo,

	UIElement_PlayButton,

	UIElement_BubbleBig,
	UIElement_BubbleSmall,
	
	UIElement_IconHome,
	UIElement_IconReplay,

	UIElement_SolidRed,
	UIElement_SolidYellow
} ui_element_type;

typedef enum {
	UIElementState_Normal,
	UIElementState_Clicked,

	UIElementState_Count
} ui_element_state;

typedef struct ui_element {
	rect Rect;
	v2 Position;
	ui_element_type Type;
	ui_element_state State;
	u32 BitmapIndex[UIElementState_Count];

	b32 IsEnabled;
} ui_element;

#define UIElementCapacity 16

typedef struct ui_controller {
	asset_manager * AssetManager;

	u32 ElementCount;
	ui_element Elements[UIElementCapacity];
} ui_controller;
