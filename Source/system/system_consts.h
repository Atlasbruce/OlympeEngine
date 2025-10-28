#pragma once

enum class EventType
{
    EventType_Default = -1,
    // -------- GAME OBJECTS MESSAGES ----------
    EventType_Message_Activate = 0,
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

    // -------- ENGINE SYSTEM EVENTS ----------
    EventType_System_NewObject, // WARNING when send message the object is not created and setup yet you must retrieve it on the next engine cycle by its name
    EventType_NewObjectPersistant,
    EventType_NewObjectStandAlone,
    EventType_NewObjectGUI,

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


    EventType_Any,
    EventType_MAX
};
