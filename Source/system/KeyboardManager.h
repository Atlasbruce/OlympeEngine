#pragma once

#include "../object.h"
#include "EventManager.h"
#include "message.h"
#include <SDL3/SDL.h>
#include <mutex>
#include "system_utils.h"

class KeyboardManager : public Object
{
public:
    KeyboardManager()
    {
        name = "KeyboardManager";
		Initialize();
    }
    virtual ~KeyboardManager()
    {
		Shutdown();
	}

    virtual ObjectType GetObjectType() const { return ObjectType::Singleton; }

    static KeyboardManager& GetInstance();
    static KeyboardManager& Get() { return GetInstance(); }

    void Initialize();
    void Shutdown();

    void HandleEvent(const SDL_Event* ev);

private:
    std::mutex m_mutex;
    void PostKeyEvent(SDL_KeyboardEvent const& ke);
};
