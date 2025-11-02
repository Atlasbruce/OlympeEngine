#pragma once

#include "Object.h"
#include "system/message.h"
#include <memory>

// Property processing stages in order of execution
enum class PropertyStage
{
    OnEvent = 0, // processed asynchronously via EventManager before update loop
    Physics = 1,
    AI = 2,
    Render = 3,
    Audio = 4,
    Count = 5
};
//---------------------------------------------------------------------------------------------
class GameObjectProperty : public Object
{
public:
    explicit GameObjectProperty(Object* owner) : owner(owner) {}
    virtual ~GameObjectProperty() = default;

    Object* GetOwner() const { return owner; }

    // Which stage this property participates in
    virtual PropertyStage Stage() const = 0;

    // Called during the update loop (for Physics/AI/Audio)
    virtual void Process() {}

    // Called during the render stage (for Render properties)
    virtual void Render() {}

    // Called when a message is delivered to this property (via owner forwarding)
    virtual void OnEvent(const Message& /*msg*/) {}

protected:
    Object* owner = nullptr;
    static float& fDt; // reference to global frame delta time
};

//---------------------------------------------------------------------------------------------
// Inherited properties - minimal implementations

class PhysicsProperty : public GameObjectProperty
{
public:
    explicit PhysicsProperty(Object* owner) : GameObjectProperty(owner) {}
    virtual ~PhysicsProperty() override = default;
    virtual PropertyStage Stage() const override { return PropertyStage::Physics; }
    virtual void Process() override
    {
        // Basic physics integration should be implemented by project-specific code.
    }
};
//---------------------------------------------------------------------------------------------
class AIProperty : public GameObjectProperty
{
public:
    explicit AIProperty(Object* owner) : GameObjectProperty(owner) {}
    virtual ~AIProperty() override = default;
    virtual PropertyStage Stage() const override { return PropertyStage::AI; }
    virtual void Process() override
    {
        // AI logic goes here.

    }
};
//---------------------------------------------------------------------------------------------
class VisualProperty : public GameObjectProperty
{
public:
    explicit VisualProperty(Object* owner) : GameObjectProperty(owner) {}
    virtual ~VisualProperty() override = default;
    virtual PropertyStage Stage() const override { return PropertyStage::Render; }
    virtual void Render() override
    {
        // Drawing logic goes here.
    }
};
//---------------------------------------------------------------------------------------------
class AudioProperty : public GameObjectProperty
{
public:
    explicit AudioProperty(Object* owner) : GameObjectProperty(owner) {}
    virtual ~AudioProperty() override = default;
    virtual PropertyStage Stage() const override { return PropertyStage::Audio; }
    virtual void Process() override
    {
        // Audio update logic goes here.
    }
};
