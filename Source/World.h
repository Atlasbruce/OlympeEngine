#pragma once

#include "Singleton.h"
#include "GameObjectProperty.h"
#include "system/EventManager.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <type_traits>

#include "Level.h" // add Level management
#include "GameState.h"

class World : public Singleton
{
public:
    World()
    {
        SYSTEM_LOG << "World Initialized\n";
    }
    virtual ~World()
    {
        // Clean up all objects
        for (auto obj : objectlist)
        {
            delete obj;
        }
        objectlist.clear();
		SYSTEM_LOG << "World Destroyed\n";
    }

    static World& GetInstance()
    {
        static World instance;
        return instance;
    }
    static World& Get() { return GetInstance(); }

    //---------------------------------------------------------------------------------------------
    // Level management
    void AddLevel(std::unique_ptr<Level> level)
    {
        if (level) m_levels.push_back(std::move(level));
    }

    const std::vector<std::unique_ptr<Level>>& GetLevels() const { return m_levels; }
    //---------------------------------------------------------------------------------------------
    //// Create and attach a property to an owner. World takes ownership.
    //template<typename T, typename... Args>
    //T* CreateProperty(Object* owner, Args&&... args)
    //{
    //    static_assert(std::is_base_of<GameObjectProperty, T>::value, "T must derive from GameObjectProperty");
    //    if (!owner) return nullptr;
    //    std::unique_ptr<GameObjectProperty> p = std::make_unique<T>(owner, std::forward<Args>(args)...);
    //    GameObjectProperty* raw = p.get();
    //    m_stageLists[static_cast<size_t>(raw->Stage())].push_back(raw);
    //    m_ownerMap[owner->GetUID()].push_back(raw);
    //    m_properties.push_back(std::move(p));
    //    return static_cast<T*>(raw);
    //}
    ////---------------------------------------------------------------------------------------------
    //// Get properties attached to an owner (by UID)
    //std::vector<GameObjectProperty*> GetPropertiesForOwner(uint64_t ownerUid) const
    //{
    //    auto it = m_ownerMap.find(ownerUid);
    //    if (it == m_ownerMap.end()) return {};
    //    return it->second;
    //}
    ////---------------------------------------------------------------------------------------------
    //std::vector<GameObjectProperty*> GetPropertiesForOwner(Object* owner) const
    //{
    //    if (!owner) return {};
    //    return GetPropertiesForOwner(owner->GetUID());
    //}
    ////---------------------------------------------------------------------------------------------
    //// Dispatch a message to all properties of an owner
    //void DispatchToProperties(Object* owner, const Message& msg)
    //{
    //    if (!owner) return;
    //    auto it = m_ownerMap.find(owner->GetUID());
    //    if (it == m_ownerMap.end()) return;
    //    for (auto* prop : it->second)
    //    {
    //        if (prop) prop->OnEvent(msg);
    //    }
    //}
    ////---------------------------------------------------------------------------------------------
    //// Remove and destroy all properties for an owner
    //void RemovePropertiesForOwner(Object* owner)
    //{
    //    if (!owner) return;
    //    auto oit = m_ownerMap.find(owner->GetUID());
    //    if (oit == m_ownerMap.end()) return;
    //    auto list = oit->second; // copy pointers to remove
    //    // remove from stage lists
    //    for (auto* prop : list)
    //    {
    //        if (!prop) continue;
    //        auto stageIndex = static_cast<size_t>(prop->Stage());
    //        auto &vec = m_stageLists[stageIndex];
    //        vec.erase(std::remove(vec.begin(), vec.end(), prop), vec.end());
    //        // remove from m_properties (unique_ptr) by pointer
    //        auto pit = std::find_if(m_properties.begin(), m_properties.end(), [prop](const std::unique_ptr<GameObjectProperty>& up){ return up.get() == prop; });
    //        if (pit != m_properties.end()) m_properties.erase(pit);
    //    }
    //    // erase owner map
    //    m_ownerMap.erase(oit);
    //}
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
            for (auto* prop : m_stageLists[static_cast<size_t>(PropertyStage::Physics)])
            {
                if (prop) prop->Process();
            }
        }

        //2) AI
        if (!paused)
        {
            for (auto* prop : m_stageLists[static_cast<size_t>(PropertyStage::AI)])
            {
                if (prop) prop->Process();
            }
        }

        //3) Render stage (note: actual drawing may require renderer context)
        for (auto* prop : m_stageLists[static_cast<size_t>(PropertyStage::Render)])
        {
            if (prop) prop->Render();
        }

        //4) Audio
        if (!paused)
        {
            for (auto* prop : m_stageLists[static_cast<size_t>(PropertyStage::Audio)])
            {
                if (prop) prop->Process();
            }
        }
    }

	//---------------------------------------------------------------------------------------------
    void AddObject(Object* obj)
    {
        // Implementation to add object to the game engine
        objectlist.push_back(obj);
    }

    // provide access to object list for other systems (Factory)
    std::vector<Object*>& GetObjectList() { return objectlist; }
    const std::vector<Object*>& GetObjectList() const { return objectlist; }

private:
    //-------------------------------------------------------------
    // Object Management
    std::vector<Object*> objectlist;

	//-------------------------------------------------------------
	// Property Management
    std::vector<std::unique_ptr<GameObjectProperty>> m_properties; // owns all properties
    std::array<std::vector<GameObjectProperty*>, static_cast<size_t>(PropertyStage::Count)> m_stageLists;
    std::unordered_map<uint64_t, std::vector<GameObjectProperty*>> m_ownerMap;

	//-------------------------------------------------------------
    // Level storage
    std::vector<std::unique_ptr<Level>> m_levels;
};
