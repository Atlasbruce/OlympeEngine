#pragma once

#include "../object.h"
#include "EventManager.h"
#include "message.h"
#include <SDL3/SDL.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>

class JoystickManager : public Object
{
public:
    JoystickManager()
    {
        name = "JoystickManager";
		Initialize();
    }
    virtual ~JoystickManager()
    {
		Shutdown();
	}

    virtual ObjectType GetObjectType() const { return ObjectType::Singleton; }

    static JoystickManager& GetInstance();
    static JoystickManager& Get() { return GetInstance(); }

    // Initialize joystick subsystem and open currently connected devices
    void Initialize();
    void Shutdown();

    // Process per-frame (optional)
    void Process(float dt);

    // Handle an incoming SDL_Event (forwarded from the application event pump)
    void HandleEvent(const SDL_Event* ev);

    // Query
    std::vector<SDL_JoystickID> GetConnectedJoysticks();
    bool IsJoystickConnected(SDL_JoystickID id);

private:
    struct JoystickInfo
    {
        SDL_Joystick* joystick = nullptr;
        SDL_JoystickID id = 0;
        std::string name;
        int numAxes = 0;
        int numButtons = 0;
        std::vector<Sint16> axes;
        std::vector<bool> buttons;
    };

    std::unordered_map<SDL_JoystickID, JoystickInfo> m_joysticks;
    std::mutex m_mutex;

    void OpenJoystick(SDL_JoystickID instance_id);
    void CloseJoystick(SDL_JoystickID instance_id);
    void PostJoystickButtonEvent(SDL_JoystickID which, int button, bool down);
    void PostJoystickAxisEvent(SDL_JoystickID which, int axis, Sint16 value);
    void PostJoystickConnectedEvent(SDL_JoystickID which, bool bconnected);
};
