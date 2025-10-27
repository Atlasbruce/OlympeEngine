/*
Olympe Engine V2 2025
Nicolas Chereau
nchereau@gmail.com

Purpose:
- Class representing a game object
- handles visual representation (sprite, animation, fx)
- Physics properties

*/
#pragma once
#include "Object.h"
#include <SDL3/SDL_rect.h>
#include "World.h"
#include "GameObjectProperty.h"
#include <vector>

class GameObject :
    public Object
{
public:

    GameObject() = default;
    virtual ~GameObject() override = default;

	// GameObject Properties--------------------------
    public:
		// position
		SDL_Point position = { 0, 0 };
        // Size
        float width = 0.0f;
        float height = 0.0f;
        // Bounding Box
	    SDL_FRect boundingBox = { 0.0f, 0.0f, 0.0f, 0.0f };

    // Add a property to this GameObject (delegates ownership to World/ECS)
    template<typename T, typename... Args>
    T* AddProperty(Args&&... args)
    {
        return World::Get().CreateProperty<T>(this, std::forward<Args>(args)...);
    }

    // Get properties attached to this GameObject
    std::vector<GameObjectProperty*> GetProperties() const
    {
        return World::Get().GetPropertiesForOwner(uid);
    }

    // Forward received messages to properties (default behavior)
    virtual void OnEvent(const Message& msg) override
    {
        World::Get().DispatchToProperties(this, msg);
    }
};

