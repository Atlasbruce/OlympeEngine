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
#include "GameEngine.h"

class Factory: public Singleton
{
public:
    using CreatorFunction = std::function<Object* ()>;
    std::map<std::string, CreatorFunction> m_registeredCreators;

    // Per-class singleton accessors
    static Factory& GetInstance()
    {
        static Factory instance;
        return instance;
    }
    static Factory& Get() { return GetInstance(); }

    /**
     * @brief Enregistre une fonction de création pour un nom de classe donné.
     * @param className Le nom de la classe (clé).
     * @param creator La fonction de création (std::function retournant BaseObject*).
     * @return true si l'enregistrement a réussi, false sinon (déjà enregistré).
     */
    bool Register(const std::string& className, CreatorFunction creator)
    {
        auto it = m_registeredCreators.find(className);
        if (it != m_registeredCreators.end())
        {
            std::cerr << "Warning: Class '" << className << "' already registered." << std::endl;
            return false;
        }
        else
        {
            m_registeredCreators.emplace(className, creator);
            std::cout << "Class '" << className << "' registered." << std::endl;
            return true;
        }
    }

    /**
     * @brief Crée une nouvelle instance de l'objet spécifié par son nom de classe.
     * @param className Le nom de la classe à créer.
     * @return Un pointeur vers le nouvel objet BaseObject, ou nullptr si non trouvé.
     */
    Object* CreateObject(const std::string& className)
    {
        // La recherche est plus rapide que dans votre version précédente
        auto it = m_registeredCreators.find(className);
        if (it == m_registeredCreators.end())
        {
            std::cerr << "Error: Class '" << className << "' not found in factory." << std::endl;
            return nullptr;
        }
        // Appel de la fonction de création stockée dans la map
        Object *o = it->second();
        // add to game engine
        GameEngine::Get().AddObject(o);
        return o;
    }

   Factory() = default;
   virtual ~Factory() {}
   Factory(const Factory&) = delete;
};


// --- Mécanisme d'Enregistrement Automatique ---

// Fonction générique pour créer une instance de classe T
template <typename T>
Object* createT()
{
    // C++ moderne: retourne T* casté en BaseObject*
    return new T();
}

/**
 * @brief Classe utilitaire qui enregistre la classe T dans la fabrique
 * lors de son initialisation statique.
 * @note Le mécanisme d'enregistrement se fait dans le constructeur de ce helper,
 * qui est appelé statiquement au démarrage du programme.
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
 * @brief Macro pour enregistrer automatiquement une classe dérivée.
 * Doit être placé dans le fichier .cpp de la classe.
 */
#define REGISTER_OBJECT(ClassName) \
    namespace { \
        AutoRegister<ClassName> RegisterHelper_##ClassName(#ClassName); \
    }


