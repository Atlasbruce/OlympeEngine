#include "MouseManager.h"
#include "EventManager.h"
#include <iostream>

MouseManager::MouseManager()
{
}

MouseManager::~MouseManager()
{
    Shutdown();
}

MouseManager& MouseManager::GetInstance()
{
    static MouseManager instance;
    return instance;
}

void MouseManager::Initialize()
{
}

void MouseManager::Shutdown()
{
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
    Message msg(be.down ? EventType::EventType_Mouse_ButtonDown : EventType::EventType_Mouse_ButtonUp, this);
    msg.deviceId = static_cast<int>(be.which);
    msg.controlId = static_cast<int>(be.button);
    msg.state = be.down ? 1 : 0;
    msg.value = be.x;
    msg.value2 = be.y;

    EventManager::Get().PostMessage(msg);
}

void MouseManager::PostMotionEvent(const SDL_MouseMotionEvent& me)
{
    Message msg(EventType::EventType_Mouse_Motion, this);
    msg.deviceId = static_cast<int>(me.which);
    msg.controlId = 0;
    msg.state = 0;
    msg.value = me.x;
    msg.value2 = me.y;

    EventManager::Get().PostMessage(msg);
}
