#pragma once

#include "../Singleton.h"
#include "EventManager.h"
#include "message.h"
#include <SDL3/SDL.h>
#include <mutex>

class MouseManager : public Singleton
{
public:
    MouseManager();
    virtual ~MouseManager();

    static MouseManager& GetInstance();
    static MouseManager& Get() { return GetInstance(); }

    void Initialize();
    void Shutdown();

    void HandleEvent(const SDL_Event* ev);

private:
    std::mutex m_mutex;
    void PostButtonEvent(const SDL_MouseButtonEvent& be);
    void PostMotionEvent(const SDL_MouseMotionEvent& me);
};
