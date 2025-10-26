/*
Olympe Engine V2 2025
Nicolas Chereau
nchereau@gmail.com

Purpose:
- Class representing a game object with position, velocity, and acceleration using SDL_FPoint.

*/
#pragma once
#include "Object.h"
#include <SDL3/SDL_rect.h>
class GameObject :
    public Object
{
public:

    GameObject() = default;

   // position SDL 3D vector
    SDL_FPoint position = { 0.0f, 0.0f  };
    // velocity SDL vector
    SDL_FPoint velocity = { 0.0f, 0.0f  };
    // acceleration SDL vector
	SDL_FPoint acceleration = { 0.0f, 0.0f };
};

