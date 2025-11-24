#pragma once

#include "ObjectComponent.h"
#include "system/EventManager.h"
#include <SDL3/SDL.h>
#include <mutex>

// AI_Player: component that listens to input events (keyboard / joystick)
// and moves its owner GameObject in the four cardinal directions.
class AI_Player : public AIComponent
{
public:
    static bool FactoryRegistered;
    static ObjectComponent* Create(void);

    explicit AI_Player();
    virtual ~AI_Player() override;

	virtual void SetOwner(Object* _owner) override;

    // AI properties participate in the AI stage (AIProperty already does this)
    virtual void Process() override;
    virtual void OnEvent(const Message& msg) override;

	static int iPlayerCounterID; // static counter to assign unique player IDs per AI_Player instance
	int m_PlayerID = -1; // unique player ID assigned to this AI_Player instance
	int m_ControllerID = -1; // controller instance ID assigned to this AI_Player instance

private:

    float m_speed = 120.0f; // for tests to be removed

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
