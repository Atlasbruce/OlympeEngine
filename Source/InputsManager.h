#pragma once
#include "Singleton.h"
#include "system/JoystickManager.h"
#include "system/KeyboardManager.h"
#include "system/MouseManager.h"

class InputsManager :
    public Singleton
{
public:
    InputsManager() = default;
    virtual ~InputsManager() = default;

    static InputsManager& GetInstance()
    {
        static InputsManager instance;
        return instance;
    }
    static InputsManager& Get() { return GetInstance(); }

    void Initialize()
    {
        JoystickManager::Get().Initialize();
        KeyboardManager::Get().Initialize();
        MouseManager::Get().Initialize();
    }

    void Shutdown()
    {
        JoystickManager::Get().Shutdown();
        KeyboardManager::Get().Shutdown();
        MouseManager::Get().Shutdown();
    }
};

