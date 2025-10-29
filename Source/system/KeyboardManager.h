#pragma once

#include "../Singleton.h"
#include "EventManager.h"
#include "message.h"
#include <SDL3/SDL.h>
#include <mutex>
#include "system_utils.h"

class KeyboardManager : public Singleton
{
public:
    KeyboardManager()
    {
        name = "KeyboardManager";
		Initialize();
		SYSTEM_LOG << "KeyboardManager created and Initialized\n";
    }
    virtual ~KeyboardManager()
    {
		Shutdown();
        SYSTEM_LOG << "KeyboardManager destroyed\n";
	}

    static KeyboardManager& GetInstance();
    static KeyboardManager& Get() { return GetInstance(); }

    void Initialize();
    void Shutdown();

    void HandleEvent(const SDL_Event* ev);

private:
    std::mutex m_mutex;
    void PostKeyEvent(SDL_KeyboardEvent const& ke);
};
