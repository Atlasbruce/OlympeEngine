#include "InputsManager.h"
#include "system/message.h"
#include "system/eventmanager.h"

using EM = ::EventManager;

InputsManager::InputsManager()
{
    name = "InputsManager";

	EM::Get().Register(this, EventType::Olympe_EventType_Joystick_Disconnected);
    EM::Get().Register(this, EventType::Olympe_EventType_Joystick_Connected);
	EM::Get().Register(this, EventType::Olympe_EventType_Keyboard_Connected);
	EM::Get().Register(this, EventType::Olympe_EventType_Keyboard_Disconnected);


    SYSTEM_LOG << "InputsManager created and Initialized\n";
}

InputsManager::~InputsManager()
{
    Shutdown();
    SYSTEM_LOG << "InputsManager destroyed\n";
}

void InputsManager::HandleEvent(const SDL_Event* ev)
{
    JoystickManager::Get().HandleEvent(ev);
    KeyboardManager::Get().HandleEvent(ev);
    MouseManager::Get().HandleEvent(ev);
}

void InputsManager::OnEvent(const Message& msg)
{
    switch (msg.msg_type)
    {
        case EventType::Olympe_EventType_Joystick_Connected:
        {
			// Auto reconnect joystick to any player that was disconnected if any
            if (GetDisconnectedPlayersCount() > 0)
            {
				// get 1st Disconnected player
				short disconnectedPlayerID = GetFirstDisconnectedPlayerID();
				if (disconnectedPlayerID >= 0)
                {
                    SYSTEM_LOG << "InputsManager: try rebinding joystick ID=" << msg.deviceId << " to disconnected player " << disconnectedPlayerID << "\n";
                    //BindControllerToPlayer(disconnectedPlayerID, msg.deviceId);
                    if (AutoBindControllerToPlayer(disconnectedPlayerID))
                    {
						// we can remove the disconnected player now, since he is rebound
						RemoveDisconnectedPlayer(disconnectedPlayerID);
						SYSTEM_LOG << "InputsManager: Joystick ID=" << msg.deviceId << " rebound to player " << disconnectedPlayerID << "\n";
                    }
                }
			}
            
			SYSTEM_LOG << "InputsManager: Joystick disconnected, ID=" << msg.deviceId << "\n";
            break;
		}
    }
}
