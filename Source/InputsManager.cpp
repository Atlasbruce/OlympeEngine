#include "InputsManager.h"


void InputsManager::HandleEvent(const SDL_Event* ev)
{
    JoystickManager::Get().HandleEvent(ev);
    KeyboardManager::Get().HandleEvent(ev);
    MouseManager::Get().HandleEvent(ev);
}
