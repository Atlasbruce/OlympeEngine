#include "MouseManager.h"
#include "EventManager.h"
#include <iostream>

MouseManager& MouseManager::GetInstance()
{
    static MouseManager instance;
    return instance;
}

void MouseManager::Initialize()
{
	name = "MouseManager";
    SYSTEM_LOG << "MouseManager Initialized\n";
}

void MouseManager::Shutdown()
{
	SYSTEM_LOG << "MouseManager Shutdown\n";
}

void MouseManager::HandleEvent(const SDL_Event* ev)
{
    if (!ev) return;
    switch (ev->type)
    {
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            PostButtonEvent(ev->button);
            break;
        case SDL_EVENT_MOUSE_MOTION:
            PostMotionEvent(ev->motion);
            break;
        default:
            break;
    }
}

void MouseManager::PostButtonEvent(const SDL_MouseButtonEvent& be)
{
    Message msg;
    msg.msg_type = be.down ? EventType::Olympe_EventType_Mouse_ButtonDown : EventType::Olympe_EventType_Mouse_ButtonUp;
    msg.sender = this;
    msg.deviceId = static_cast<int>(be.which);
    msg.controlId = static_cast<int>(be.button);
    msg.state = be.down ? 1 : 0;
    msg.value = be.x;
    msg.value2 = be.y;

    EventManager::Get().AddMessage(msg);
}

void MouseManager::PostMotionEvent(const SDL_MouseMotionEvent& me)
{
    Message msg;
    msg.msg_type = EventType::Olympe_EventType_Mouse_Motion;
    msg.sender = this;
    msg.deviceId = static_cast<int>(me.which);
    msg.controlId = 0;
    msg.state = 0;
    msg.value = me.x;
    msg.value2 = me.y;

    EventManager::Get().AddMessage(msg);
}
