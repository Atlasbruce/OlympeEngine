#pragma once
#include "Singleton.h"
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
};

