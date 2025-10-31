#pragma once

#include "GameObjectProperty.h"
#include "system/EventManager.h"
#include <SDL3/SDL.h>
#include <mutex>

// AI_Player: property that listens to input events (keyboard / joystick)
// and moves its owner GameObject in the four cardinal directions.
class AI_Player : public AIProperty
{
public:
    explicit AI_Player(Object* owner, float speed = 120.0f);
    virtual ~AI_Player() override;

    // AI properties participate in the AI stage (AIProperty already does this)
    virtual void Process() override;
    virtual void OnEvent(const Message& msg) override;

private:
    float m_speed = 120.0f;
    float m_posX = 0.0f;
    float m_posY = 0.0f;

    // input state
    float m_axisX = 0.0f;
    float m_axisY = 0.0f;
    bool m_keyUp = false;
    bool m_keyDown = false;
    bool m_keyLeft = false;
    bool m_keyRight = false;

    float m_deadzone = 0.2f;
    std::mutex m_mutex;
};
