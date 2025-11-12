#pragma once

constexpr double k_PI = 3.14159265358979323846;

// Configuration: defaults
static const int DEFAULT_WINDOW_WIDTH = 1280;
static const int DEFAULT_WINDOW_HEIGHT = 720;

enum class EventType
{
    EventType_Default = -1,
    // -------- GAME OBJECTS MESSAGES ----------
	EventType_Message_Activate = 0, // object activation 
    EventType_Message_Deactivate,
    EventType_Message_CollideEvent,
    EventType_Message_UncollideEvent,
    EventType_Message_CollideNav,
    EventType_Message_UnCollideNav,
    EventType_Message_CollideDeathZone, // collision with death zone

	//-------- MENU SYSTEM EVENTS ----------
	EventType_Menu_Enter,
	EventType_Menu_Exit,
	EventType_Menu_Validate,

    EventType_Hit,

    // -------- OBBJECT EVENTS ----------
    EventType_Object_Create, 
	EventType_Object_Destroy,
	EventType_Property_Add,
	EventType_Property_Remove,

	// -------- LEVEL EVENTS ----------
	EventType_Level_Load,
	EventType_Level_Unload,

	// -------- SECTOR EVENTS ----------    
    EventType_SectorToActivate,        // Sector to activate
    EventType_SectorToDeactivate,      // Sector to deactivate

	// -------- INPUTS EVENTS ----------
	EventType_Joystick_AxisMotion,
	EventType_Joystick_ButtonDown,
	EventType_Joystick_ButtonUp,

	EventType_Keyboard_KeyDown,
	EventType_Keyboard_KeyUp,

	EventType_Mouse_ButtonDown,
	EventType_Mouse_ButtonUp,
	EventType_Mouse_Motion,

	// -------- CAMERA EVENT ----------
	EventType_Camera_Shake, // shake camera with given parameters intensity, duration (milliseconds)
	EventType_Camera_Teleport, // instantly move camera to position of the renderer scene
	EventType_Camera_MoveToPosition, // smoothly move camera to position over time using blending (velocity)
	EventType_Camera_ZoomTo, // smoothly zoom camera to level over time using blending (velocity)
	EventType_Camera_Reset, // reset camera to default position/zoom
	EventType_Camera_Mode_SetBounds, // set camera movement bounds rectangle
	EventType_Camera_Mode_2D, // set camera to free 2D mode
	EventType_Camera_Mode_2_5D, // set camera to 2.5D mode (follow target on X axis only)
	EventType_Camera_Mode_Isometric, // set camera to isometric mode
	EventType_Camera_Target_Follow, // follow an object (given by UID or object pointer or object name)
	EventType_Camera_Target_Unfollow, // stop following object, return to free mode
	EventType_Camera_Target_SetOffset, // set camera target offset (from followed object position)
	EventType_Camera_Target_ClearOffset, // clear camera target offset (return to followed object position)
	EventType_Camera_Viewport_Add, // add a camera viewport according to the number of renderer textures on screen
	EventType_Camera_Viewport_Remove, // remove a camera viewport from renderers texture list on screen
	EventType_Camera_Viewport_Clear, // clear camera viewport (return to full renderer size)

	// -------- GAME EVENTS ----------
	EventType_Game_Pause, // pause the game from GameMenu
	EventType_Game_Resume, // resume the game from GameMenu
	EventType_Game_Quit, // quit the game from GameMenu
	EventType_Game_Restart, // restart the current level from GameMenu
	EventType_Game_AddPlayer, // add a new player (up to 4) param: player id (0..3)
	EventType_Game_RemovePlayer, // remove a player param: player id (0..3)
	EventType_Game_TakeScreenshot, // take a screenshot of the current frame
	EventType_Game_SaveState, // save game state to slot (param: slot id)
	EventType_Game_LoadState, // load game state from slot (param: slot id)


    EventType_Any,
    EventType_MAX
};
