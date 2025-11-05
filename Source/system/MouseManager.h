#pragma once

#include "../object.h"
#include "EventManager.h"
#include "message.h"
#include <SDL3/SDL.h>
#include <mutex>
#include "system_utils.h"

class MouseManager : public Object
{
public:
    MouseManager()
    {
		Initialize();
    }
    virtual ~MouseManager()
    {
        Shutdown();
    }

    virtual ObjectType GetObjectType() const { return ObjectType::Singleton; }

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
