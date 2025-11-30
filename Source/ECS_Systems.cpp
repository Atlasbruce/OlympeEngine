/*
Olympe Engine V2 - 2025
Nicolas Chereau
nchereau@gmail.com

This file is part of Olympe Engine V2.

ECS Systems purpose: Define systems that operate on entities with specific components.

*/

#include "ECS_Systems.h"
#include "ECS_Components.h"
#include "ECS_Register.h"
#include "ECS_Entity.h"
#include "World.h" 
#include "GameEngine.h" // For delta time (fDt)
#include <iostream>
#include <bitset>

ECS_System::ECS_System() : requiredSignature()
{
    // Define the required components: Position AND AI_Player
    requiredSignature.set(GetComponentTypeID_Static<_Position>(), true);
    requiredSignature.set(GetComponentTypeID_Static<_AI_Player>(), true);
}

void ECS_System::Process()
{
    // Iterate ONLY over the relevant entities stored in m_entities
    for (EntityID entity : m_entities)
    {
        try
        {
            // Direct and fast access to Component data from the Pools
            _Position& pos = World::Get().GetComponent<_Position>(entity);
            const _AI_Player& ai = World::Get().GetComponent<_AI_Player>(entity);

            // Game logic: simple movement based on speed and delta time
            pos.x += ai.speed * GameEngine::fDt;
        }
        catch (const std::exception& e)
        {
            std::cerr << "AISystem Error for Entity " << entity << ": " << e.what() << "\n";
        }
    }
}
