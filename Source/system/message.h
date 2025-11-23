#pragma once

#include "system_consts.h"
#include <string>
#include <cstdint>
#include "SDL_events.h"
#include "windows.h"

// Message struct in the global namespace. It contains an EventStructType so
// existing code that uses "msg.type" continues to work. Additional fields
// are provided for input events (joystick/keyboard/mouse) so message payload
// can be transported via the EventManager.
struct Message
{
	EventStructType struct_type = EventStructType::EventStructType_Olympe;
	EventType msg_type = EventType::Olympe_EventType_Any; // optional message identifier (e.g. OlympeMessage)
	MSG *msg = nullptr; // optional Win32 MSG structure
	SDL_Event* sdlEvent = nullptr; // optional SDL_Event structure

    // Additional convenience fields for engine-level messages
    void* sender = nullptr; // optional sender pointer
    uint64_t targetUid = 0; // target object UID for operations (create/destroy/add property)
    std::string className; // class to create (for object creation)
    std::string objectName; // desired object name
    std::string ComponentType; // property type identifier (for property add/remove)
    std::string propertyParams; // optional params serialized as string

    // Generic integer / float payload fields. For input events these are used as:
    //  - deviceId : joystick instance id, keyboard id, mouse id
    //  - controlId: button index or axis index or scancode
    //  - state    : button pressed (1) / released (0) or other integer state
    //  - value    : axis value normalized to [-1,1] or primary float payload
    //  - value2   : secondary float payload (e.g. mouse Y coordinate)
    int deviceId = -1;
    int controlId = -1;
    int state = 0;
    float value = 0.0f;
    float value2 = 0.0f;

    void* payload = nullptr; // optional pointer for extended data
};
