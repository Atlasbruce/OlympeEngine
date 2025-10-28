#pragma once

#include "../Singleton.h"
#include "EventManager.h"
#include "message.h"
#include <SDL3/SDL.h>
#include <mutex>

class KeyboardManager : public Singleton
{
public:
    KeyboardManager();
    virtual ~KeyboardManager();

    static KeyboardManager& GetInstance();
    static KeyboardManager& Get() { return GetInstance(); }

    void Initialize();
    void Shutdown();

    void HandleEvent(const SDL_Event* ev);

private:
    std::mutex m_mutex;
    void PostKeyEvent(SDL_KeyboardEvent const& ke);
};
