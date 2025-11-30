/*
Olympe Engine V2 - 2025
Nicolas Chereau
nchereau@gmail.com

This file is part of Olympe Engine V2.

ECS Systems purpose: Define systems that operate on entities with specific components.

*/

#pragma once

#include "Ecs_Entity.h"
#include <set>


// The System class handles game logic over entities with specific components.
class ECS_System
{
public:
    // The signature required for an Entity to be processed by this System
    ComponentSignature requiredSignature;

    // The set of Entities this System processes in its Update loop
    std::set<EntityID> m_entities;

    ECS_System();

    // The core logic of the System
    void Process();

    void AddEntity(EntityID entity) { m_entities.insert(entity); }
    void RemoveEntity(EntityID entity) { m_entities.erase(entity); }
};
