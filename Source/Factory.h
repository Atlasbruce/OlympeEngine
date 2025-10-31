/*
Olympe Engine V2 2025
Nicolas Chereau
nchereau@gmail.com

Purpose:
- Class that enables the creation of various game objects.

*/
#pragma once
#include "Singleton.h"
#include "Object.h"
#include <map>
#include <string>
#include <functional>
#include <memory>
#include <stdexcept>
#include <iostream>
#include "World.h"
#include "system/system_utils.h"
#include "system/EventManager.h"
#include "GameObjectProperty.h"

class Factory: public Singleton
{
public:
    using CreatorFunction = std::function<Object* ()>;
    std::map<std::string, CreatorFunction> m_registeredCreators;

    Factory()
    {
		name = "Factory";
		SYSTEM_LOG << "Factory created and Initialized\n";
		// register to event manager to receive object events
		EventManager::Get().Register(this, EventType::EventType_Object_Create);
		EventManager::Get().Register(this, EventType::EventType_Object_Destroy);
		EventManager::Get().Register(this, EventType::EventType_Property_Add);
		EventManager::Get().Register(this, EventType::EventType_Property_Remove);
    };
    virtual ~Factory()
    {
        SYSTEM_LOG << "Factory destroyed\n";
		EventManager::Get().Unregister(this, EventType::EventType_Object_Create);
		EventManager::Get().Unregister(this, EventType::EventType_Object_Destroy);
		EventManager::Get().Unregister(this, EventType::EventType_Property_Add);
		EventManager::Get().Unregister(this, EventType::EventType_Property_Remove);
	}

    // Per-class singleton accessors
    static Factory& GetInstance()
    {
        static Factory instance;
        return instance;
    }
    static Factory& Get() { return GetInstance(); }

    /**
     * @brief Enregistre une fonction de cr�ation pour un nom de classe donn�.
     * @param className Le nom de la classe (cl�).
     * @param creator La fonction de cr�ation (std::function retournant BaseObject*).
     * @return true si l'enregistrement a r�ussi, false sinon (d�j� enregistr�).
     */
    bool Register(const std::string& className, CreatorFunction creator)
    {
        auto it = m_registeredCreators.find(className);
        if (it != m_registeredCreators.end())
        {
            SYSTEM_LOG << "Warning: Class '" << className << "' already registered." << std::endl;
            return false;
        }
        else
        {
            m_registeredCreators.emplace(className, creator);
            SYSTEM_LOG << "Class '" << className << "' registered." << std::endl;
            return true;
        }
    }

    /**
     * @brief Cr�e une nouvelle instance de l'objet sp�cifi� par son nom de classe.
     * @param className Le nom de la classe � cr�er.
     * @return Un pointeur vers le nouvel objet BaseObject, ou nullptr si non trouv�.
     */
    Object* CreateObject(const std::string& className)
    {
        auto it = m_registeredCreators.find(className);
        if (it == m_registeredCreators.end())
        {
            SYSTEM_LOG << "Error: Class '" << className << "' not found in factory." << std::endl;
            return nullptr;
        }
        Object *o = it->second();
        World::Get().AddObject(o);
        return o;
    }

    // Event handling: respond to create/destroy/property messages
    virtual void OnEvent(const Message& msg) override
    {
        switch (msg.type)
        {
        case EventType::EventType_Object_Create:
            {
                if (!msg.className.empty())
                {
                    Object* o = CreateObject(msg.className);
                    if (o)
                    {
                        if (!msg.objectName.empty()) o->name = msg.objectName;
                        // return the created object's UID via payload pointer (not ideal but works)
                        // we can't push directly a message back synchronously here; instead post a system message
                        Message res(EventType::EventType_Default, this);
                        res.targetUid = o->GetUID();
                        res.objectName = o->name;
                        EventManager::Get().PostMessage(res);
                        SYSTEM_LOG << "Factory created object '" << o->name << "' uid=" << o->GetUID() << "\n";
                    }
                }
                break;
            }
        case EventType::EventType_Object_Destroy:
            {
                uint64_t uid = msg.targetUid;
                if (uid !=0)
                {
                    // find and destroy object in world
                    // simple linear search - World could provide a helper
                    // we directly access World::objectlist isn't public; use World API if available
                    // For now iterate and delete
                    auto &list = GetWorldObjectList();
                    for (auto it = list.begin(); it != list.end(); ++it)
                    {
                        if ((*it)->GetUID() == uid)
                        {
                            delete *it;
                            list.erase(it);
                            SYSTEM_LOG << "Factory destroyed object uid=" << uid << "\n";
                            break;
                        }
                    }
                }
                break;
            }
        case EventType::EventType_Property_Add:
            {
                uint64_t uid = msg.targetUid;
                if (uid !=0 && !msg.propertyType.empty())
                {
                    Object* o = FindObjectByUID(uid);
                    if (o)
                    {
                        // simplistic: propertyType is one of "Physics","AI","Visual","Audio"
                        if (msg.propertyType == "Physics") World::Get().CreateProperty<PhysicsProperty>(o);
                        else if (msg.propertyType == "AI") World::Get().CreateProperty<AIProperty>(o);
                        else if (msg.propertyType == "Visual") World::Get().CreateProperty<VisualProperty>(o);
                        else if (msg.propertyType == "Audio") World::Get().CreateProperty<AudioProperty>(o);
                        SYSTEM_LOG << "Factory added property '" << msg.propertyType << "' to object uid=" << uid << "\n";
                    }
                }
                break;
            }
        case EventType::EventType_Property_Remove:
            {
                // Not implemented: requires property type lookup and removal API in World
                SYSTEM_LOG << "Factory received Property_Remove for uid=" << msg.targetUid << " (not implemented)\n";
                break;
            }
        default:
            break;
        }
    }

private:
    // helper to find object by uid in world
    Object* FindObjectByUID(uint64_t uid)
    {
        // access world's object list via reflection - World doesn't expose, so we add a helper local
        auto &list = GetWorldObjectList();
        for (auto obj : list) if (obj && obj->GetUID() == uid) return obj;
        return nullptr;
    }

    // Access internal world object list via friend function (quick hack) - implement as wrapper
    static std::vector<Object*>& GetWorldObjectList()
    {
        // World doesn't expose object list; we rely on a private static accessor in World (to be added)
        return World::Get().GetObjectList();
    }
};


// --- M�canisme d'Enregistrement Automatique ---

// Fonction g�n�rique pour cr�er une instance de classe T
template <typename T>
Object* createT()
{
    // C++ moderne: retourne T* cast� en BaseObject*
    return new T();
}

/**
 * @brief Classe utilitaire qui enregistre la classe T dans la fabrique
 * lors de son initialisation statique.
 * @note Le m�canisme d'enregistrement se fait dans le constructeur de ce helper,
 * qui est appel� statiquement au d�marrage du programme.
 */
template <typename T>
class AutoRegister
{
public:
    AutoRegister(const std::string& className)
    {
        Factory::GetInstance().Register(className, createT<T>);
    }
};

/**
 * @brief Macro pour enregistrer automatiquement une classe d�riv�e.
 * Doit �tre plac� dans le fichier .cpp de la classe.
 */
#define REGISTER_OBJECT(ClassName) \
    namespace { \
        AutoRegister<ClassName> RegisterHelper_##ClassName(#ClassName); \
    }


