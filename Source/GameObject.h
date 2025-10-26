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
class GameObject :
    public Object
{
public:

    GameObject() = default;
    virtual ~GameObject() override {};

	// GameObject Properties--------------------------
    public:
		// position
		SDL_Point position = { 0.0f, 0.0f };
        // Size
        float width = 0.0f;
        float height = 0.0f;
        // Bounding Box
	    SDL_FRect boundingBox = { 0.0f, 0.0f, 0.0f, 0.0f };
};

