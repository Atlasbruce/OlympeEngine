#pragma once
#include "object.h"
#include "system/JoystickManager.h"
#include "system/KeyboardManager.h"
#include "system/MouseManager.h"
#include <unordered_map>

class InputsManager : public Object
{
public:
    InputsManager()
    {
		name = "InputsManager";
        SYSTEM_LOG << "InputsManager created and Initialized\n";
    }
    virtual ~InputsManager()
    {
        Shutdown();
		SYSTEM_LOG << "InputsManager destroyed\n";
    }

    virtual ObjectType GetObjectType() const { return ObjectType::Singleton; }

    static InputsManager& GetInstance()
    {
        static InputsManager instance;
        return instance;
    }
    static InputsManager& Get() { return GetInstance(); }

    void Shutdown()
    {
        JoystickManager::Get().Shutdown();
        KeyboardManager::Get().Shutdown();
        MouseManager::Get().Shutdown();
        m_playerBindings.clear();
        m_keyboardAssigned = false;
    }

    virtual void HandleEvent(const SDL_Event* ev);

	//--------------------------------------------------------------
	// Automatically bind first available controller (joystick or keyboard) to a player
    bool AutoBindControllerToPlayer(short playerID)
    {
        // try to bind first available joystick
        auto joysticks = JoystickManager::Get().GetConnectedJoysticks();
        for (auto jid : joysticks)
        {
            if (BindControllerToPlayer(playerID, jid)) return true;
        }

		// Failled to bind joystick,
		SYSTEM_LOG << "No available joystick to bind to player " << playerID << ". Try to bind keyboard\n";

        // try to bind keyboard if no joystick available
        if (BindControllerToPlayer(playerID, SDL_JoystickID(-1))) 
        {
			SYSTEM_LOG << "Player " << playerID << " bound to keyboard\n";
            return true;
        }
        else
        {
			SYSTEM_LOG << "Failed to bind keyboard to player " << playerID << " (already assigned?)\n";
            return false;
        }
	}
	//--------------------------------------------------------------

    // Bind a controller (joystick id) or keyboard (-1) to a player
    bool BindControllerToPlayer(short playerID, SDL_JoystickID controller)
    {
        // if controller == -1 -> keyboard
        if (controller == SDL_JoystickID(-1))
        {
            if (m_keyboardAssigned) return false;
            m_keyboardAssigned = true;
            m_playerBindings[playerID] = controller;
            return true;
        }
        // ensure joystick exists
        if (!JoystickManager::Get().IsJoystickConnected(controller)) return false;
        // ensure not already used
        for (auto &kv : m_playerBindings) if (kv.second == controller) return false;
        m_playerBindings[playerID] = controller;
		SYSTEM_LOG << "Player " << playerID << " bound to joystick " << controller << "\n";
        return true;
    }

    bool UnbindControllerFromPlayer(short playerID)
    {
        auto it = m_playerBindings.find(playerID);
        if (it == m_playerBindings.end()) return false;
        if (it->second == SDL_JoystickID(-1)) m_keyboardAssigned = false;
        m_playerBindings.erase(it);
		SYSTEM_LOG << "Player " << playerID << " unbound from controller\n";
        return true;
    }

    // Query
    bool IsPlayerBound(short playerID) const { return m_playerBindings.find(playerID) != m_playerBindings.end(); }
    SDL_JoystickID GetPlayerBinding(short playerID) const
    {
        auto it = m_playerBindings.find(playerID);
        if (it == m_playerBindings.end()) return SDL_JoystickID(0);
        return it->second;
    }

private:
    std::unordered_map<short, SDL_JoystickID> m_playerBindings;
    bool m_keyboardAssigned = false;
	JoystickManager& joystickmanager = JoystickManager::GetInstance();
	KeyboardManager& keyboardmanager = KeyboardManager::GetInstance();
	MouseManager& mousemanager = MouseManager::GetInstance();

};

