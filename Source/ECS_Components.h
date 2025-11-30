/*
Olympe Engine V2 - 2025
Nicolas Chereau
nchereau@gmail.com

This file is part of Olympe Engine V2.

Components purpose: Include all component definitions used in the ECS architecture.

*/

#pragma once

#include "Ecs_Entity.h"
#include <string>

// --- Component 1: Position (Data) ---
struct _Position
{
    float x = 0.0f;
    float y = 0.0f;
};

// --- Component 2: Sprite (Render Data) ---
struct _Sprite
{
    std::string assetID; // Texture ID
    int zIndex = 0;      // Render order
};

// --- Component 3: AI_Player (Behavior Data/Marker) ---
struct _AI_Player
{
    float speed = 50.0f; // Movement speed in pixels/second
};