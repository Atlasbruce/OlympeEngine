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
#include "World.h"
#include "ObjectComponent.h"
#include <vector>
#include <SDL3/SDL_rect.h>


enum class EntityType
{
    None = 0,
    Player,
    NPC,
    Collectible,
    Trigger,
    Projectile,
    Count
};

class GameObject : public Object
{
public:
    static bool FactoryRegistered;
    static Object* Create();
    GameObject()
    {
        name = "unnamed GameObject";
	}
    virtual ~GameObject() override = default;
	virtual ObjectType GetObjectType() const override { return ObjectType::Entity; }
	virtual EntityType GetEntityType() const { return EntityType::None; }

    //---------------------------------------------------------------------------------------------
	// GameObject Properties--------------------------
    public:
		// position
		SDL_FPoint position = { 0., 0. };
        // Size
        float width = 100.0f;
        float height = 150.0f;
        // Bounding Box
	    SDL_FRect boundingBox = { 0.0f, 0.0f, 0.0f, 0.0f };
		// static or dynamic
        bool isDynamic = false;

    // Forward received messages to properties (default behavior)
    virtual void OnEvent(const Message& msg) override
    {
       // World::Get().DispatchToProperties(this, msg);
    }

	void Render() { SYSTEM_LOG << "Rendering GameObject: " << name << " at (" << position.x << ", " << position.y << ")\n"; }

protected:
    
};

