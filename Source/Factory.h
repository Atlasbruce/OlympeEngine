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
     * @brief Cr�e une nouvelle instance de l'objet sp�cifi� par son nom de classe.
     * @param className Le nom de la classe � cr�er.
     * @return Un pointeur vers le nouvel objet BaseObject, ou nullptr si non trouv�.
     */
    Object* CreateObject(const std::string& className)
    {
        // La recherche est plus rapide que dans votre version pr�c�dente
        auto it = m_registeredCreators.find(className);
        if (it == m_registeredCreators.end())
        {
            std::cerr << "Error: Class '" << className << "' not found in factory." << std::endl;
            return nullptr;
        }
        // Appel de la fonction de cr�ation stock�e dans la map
        Object *o = it->second();
        // add to game engine
        GameEngine::Get().AddObject(o);
        return o;
    }

   Factory() = default;
   virtual ~Factory() {}
   Factory(const Factory&) = delete;
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


