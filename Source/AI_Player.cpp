#include "AI_Player.h"
#include "GameObject.h"
#include "system/EventManager.h"
#include <iostream>
#include <cmath>

AI_Player::AI_Player(Object* owner, float speed)
    : AIProperty(owner)
    , m_speed(speed)
{
    // initialize internal position from owner if it's a GameObject
    if (auto go = dynamic_cast<GameObject*>(owner))
    {
        m_posX = static_cast<float>(go->position.x);
        m_posY = static_cast<float>(go->position.y);
    }

    // Register to input-related events
    EventManager::Get().Register(this, EventType::EventType_Joystick_AxisMotion, [this](const Message& m){ this->OnEvent(m); });
    EventManager::Get().Register(this, EventType::EventType_Joystick_ButtonDown, [this](const Message& m){ this->OnEvent(m); });
    EventManager::Get().Register(this, EventType::EventType_Joystick_ButtonUp, [this](const Message& m){ this->OnEvent(m); });
    EventManager::Get().Register(this, EventType::EventType_Keyboard_KeyDown, [this](const Message& m){ this->OnEvent(m); });
    EventManager::Get().Register(this, EventType::EventType_Keyboard_KeyUp, [this](const Message& m){ this->OnEvent(m); });
}

AI_Player::~AI_Player()
{
    // Unregister all callbacks associated with this instance
    EventManager::Get().UnregisterAll(this);
}

void AI_Player::Process(float dt)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    float vx = 0.0f;
    float vy = 0.0f;

    // joystick axes take priority when outside deadzone
    if (std::fabs(m_axisX) > m_deadzone) vx = m_axisX * m_speed;
    else
    {
        if (m_keyLeft) vx = -m_speed;
        if (m_keyRight) vx = m_keyRight ? m_speed : vx;
    }

    if (std::fabs(m_axisY) > m_deadzone) vy = m_axisY * m_speed;
    else
    {
        if (m_keyUp) vy = -m_speed;
        if (m_keyDown) vy = m_keyDown ? m_speed : vy;
    }

    m_posX += vx * dt;
    m_posY += vy * dt;

    if (auto go = dynamic_cast<GameObject*>(owner))
    {
        go->position.x = static_cast<int>(std::round(m_posX));
        go->position.y = static_cast<int>(std::round(m_posY));
    }
}

void AI_Player::OnEvent(const Message& msg)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    switch (msg.type)
    {
        case EventType::EventType_Joystick_AxisMotion:
        {
            // controlId = axis index, value = normalized [-1..1]
            if (msg.controlId == 0)
                m_axisX = msg.value;
            else if (msg.controlId == 1)
                m_axisY = msg.value;
            break;
        }

        case EventType::EventType_Joystick_ButtonDown:
        case EventType::EventType_Joystick_ButtonUp:
        {
            // optionally map buttons to movement (e.g. dpad) if desired
            // msg.controlId is button index, msg.state 1=down 0=up
            // Not implemented here.
            break;
        }

        case EventType::EventType_Keyboard_KeyDown:
        {
            // msg.controlId contains SDL_Scancode
            switch (static_cast<SDL_Scancode>(msg.controlId))
            {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    m_keyUp = true; break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    m_keyDown = true; break;
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    m_keyLeft = true; break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    m_keyRight = true; break;
                default:
                    break;
            }
            break;
        }

        case EventType::EventType_Keyboard_KeyUp:
        {
            switch (static_cast<SDL_Scancode>(msg.controlId))
            {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    m_keyUp = false; break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    m_keyDown = false; break;
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    m_keyLeft = false; break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    m_keyRight = false; break;
                default:
                    break;
            }
            break;
        }

        default:
            break;
    }
}
