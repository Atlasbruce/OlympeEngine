#include "KeyboardManager.h"
#include "EventManager.h"
#include <iostream>
#include "system_utils.h"

KeyboardManager& KeyboardManager::GetInstance()
{
    static KeyboardManager instance;
    return instance;
}

void KeyboardManager::Initialize()
{
    // nothing special for now - SDL keyboard events are delivered automatically
	SYSTEM_LOG << "KeyboardManager created and Initialized\n";
}

void KeyboardManager::Shutdown()
{
    SYSTEM_LOG << "KeyboardManager deleted\n";
}

void KeyboardManager::HandleEvent(const SDL_Event* ev)
{
   // if (!ev) return; // useless because it is tested before function call
    switch (ev->type)
    {
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            PostKeyEvent(ev->key);
            break;
        default:
            break;
    }
}

void KeyboardManager::PostKeyEvent(SDL_KeyboardEvent const& ke)
{
    Message msg;
    msg.struct_type = EventStructType::EventStructType_Olympe;
    msg.msg_type = ke.down ? EventType::Olympe_EventType_Keyboard_KeyDown : EventType::Olympe_EventType_Keyboard_KeyUp;
    msg.sender = this;
    msg.deviceId = -1; // -1 is keyboard // static_cast<int>(ke.which);
    msg.controlId = static_cast<int>(ke.scancode);
    msg.state = ke.down ? 1 : 0;
    msg.param1 = 0.0f;

    EventManager::Get().AddMessage(msg);
}
