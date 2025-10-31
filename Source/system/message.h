#pragma once

#include "system_consts.h"
#include <string>
#include <cstdint>

// Message struct in the global namespace. It contains an EventType so
// existing code that uses "msg.type" continues to work. Additional fields
// are provided for input events (joystick/keyboard/mouse) so message payload
// can be transported via the EventManager.
struct Message
{
    EventType type;
    void* sender = nullptr; // optional sender pointer

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

    // Additional convenience fields for engine-level messages
    uint64_t targetUid = 0; // target object UID for operations (create/destroy/add property)
    std::string className; // class to create (for object creation)
    std::string objectName; // desired object name
    std::string propertyType; // property type identifier (for property add/remove)
    std::string propertyParams; // optional params serialized as string

    Message(EventType t = EventType::EventType_Default, void* s = nullptr)
        : type(t), sender(s), deviceId(-1), controlId(-1), state(0), value(0.0f), value2(0.0f), payload(nullptr), targetUid(0) {}
};
