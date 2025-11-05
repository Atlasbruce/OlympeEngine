#pragma once

#include "object.h"
#include "ObjectComponent.h"
#include "system/EventManager.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <type_traits>

#include "Level.h" // add Level management
#include "GameState.h"

class World : public Object
{
public:
    World()
    {
        SYSTEM_LOG << "World Initialized\n";
    }
    virtual ~World()
    {
        // Clean up all objects
        for (auto obj : m_objectlist)
        {
            delete obj;
        }
        m_objectlist.clear();
		SYSTEM_LOG << "World Destroyed\n";
    }

    virtual ObjectType GetObjectType() const { return ObjectType::Singleton; }

    static World& GetInstance()
    {
        static World instance;
        return instance;
    }
    static World& Get() { return GetInstance(); }

    //---------------------------------------------------------------------------------------------
    // Main processing loop called each frame: events are processed first (async), then stages in order
    void Process()
    {
        //0) Ensure queued events are dispatched before the update stages
        EventManager::Get().Process();

        // check global game state
        GameState state = GameStateManager::GetState();
        bool paused = (state == GameState::GameState_Paused);

        //1) Physics
        if (!paused)
        {
            for (auto* prop : array_component_lists_bytypes[static_cast<size_t>(ComponentType::Physics)])
            {
                if (prop) prop->Process();
            }
        }

        //2) AI
        if (!paused)
        {
            for (auto* prop : array_component_lists_bytypes[static_cast<size_t>(ComponentType::AI)])
            {
                if (prop) prop->Process();
            }
        }

        ////3) Render stage (note: actual drawing may require renderer context)
        //for (auto* prop : array_component_lists_bytypes[static_cast<size_t>(ComponentType::Visual)])
        //{
        //    if (prop) prop->Render();
        //}

        //4) Audio
        if (!paused)
        {
            for (auto* prop : array_component_lists_bytypes[static_cast<size_t>(ComponentType::Audio)])
            {
                if (prop) prop->Process();
            }
        }
    }
    //---------------------------------------------------------------------------------------------
    void Render()
    {
        // Render stage (note: actual drawing may require renderer context)
        for (auto* prop : array_component_lists_bytypes[static_cast<size_t>(ComponentType::Visual)])
        {
            if (prop) prop->Render();
        }
	}

	//---------------------------------------------------------------------------------------------
	// Objects & Entities management
    void StoreObject(Object* obj)
    {
        // Implementation to add object to the game engine
        m_objectlist.push_back(obj);
		SYSTEM_LOG << "World: Added object " + obj->name + " to World\n";
    }

    // provide access to object list for other systems (Factory)
    std::vector<Object*>& GetObjectList() { return m_objectlist; }
    const std::vector<Object*>& GetObjectList() const { return m_objectlist; }

    //---------------------------------------------------------------------------------------------
    // Objects' Components management
    void StoreComponent(ObjectComponent* objectComponent)
    {
        if (!objectComponent)
        {
            SYSTEM_LOG << "Error: World::AddComponent called with null component!\n";
            return;
        }

        try
        {
            //add Component to the right type list in the array
            array_component_lists_bytypes[static_cast<size_t>(objectComponent->GetComponentType())].push_back(objectComponent);
            SYSTEM_LOG << "World: Added component " + objectComponent->name + " of type " << static_cast<int>(objectComponent->GetComponentType()) << " to World\n";
        }
        catch (const std::exception&)
        {
			SYSTEM_LOG << "Error: World::AddComponent failed to add component of type " << static_cast<int>(objectComponent->GetComponentType()) << "\n";
        }
    }

    //---------------------------------------------------------------------------------------------
    // Level management
    void AddLevel(std::unique_ptr<Level> level)
    {
        if (level) m_levels.push_back(std::move(level));
    }

    const std::vector<std::unique_ptr<Level>>& GetLevels() const { return m_levels; }

private:
    //-------------------------------------------------------------
    // Entities & Object Management
    std::vector<Object*> m_objectlist;

	//-------------------------------------------------------------
	// Components Management

    // Indexed Array of lists of components by type for fast processing
	std::array<std::vector<ObjectComponent*>, static_cast<size_t>(ComponentType::Count)> array_component_lists_bytypes;

	//-------------------------------------------------------------
    // Level storage
    std::vector<std::unique_ptr<Level>> m_levels;
};
