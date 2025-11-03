#pragma once

#include "Object.h"
#include "system/message.h"
#include <memory>
#include "system/system_utils.h"

class GameObject;

// Components processing types in order of execution
enum class ComponentType
{
	None = 0, // no processing or used for extra data components for entities
    Physics = 1,
    AI = 2,
    Visual = 3,
    Audio = 4,
	Count = 5 // used in World to size arrays
};
//---------------------------------------------------------------------------------------------
class ObjectComponent : public Object
{
public:
	explicit ObjectComponent() = default;
    virtual ~ObjectComponent() = default;

    inline Object* GetEntity() const { return owner; }
	virtual void SetEntity(Object* _owner) { owner = _owner; }

    // Which Type this property participates in
    virtual ComponentType GetComponentType() const = 0;

    // Called during the update loop (for Physics/AI/Audio)
    virtual void Process() {}

    // Called during the render Type (for Render properties)
    virtual void Render() {}

    // Called when a message is delivered to this property (via owner forwarding)
    virtual void OnEvent(const Message& /*msg*/) {}

	// Set the owner object of this component
	virtual void SetOwner(Object* _owner)
    {
        owner = _owner;
        gao = (GameObject*)(owner);
    }
	inline Object* GetOwner() const { return owner; }

protected:
    Object* owner = nullptr;
    static float& fDt; // reference to global frame delta time
    GameObject* gao = nullptr;
};

//---------------------------------------------------------------------------------------------
// Inherited properties - minimal implementations

class PhysicsComponent : public ObjectComponent
{
public:
    explicit PhysicsComponent() = default;
    virtual ~PhysicsComponent() override = default;
    virtual ComponentType GetComponentType() const override { return ComponentType::Physics; }
    virtual void Process() override
    {
        // Basic physics integration should be implemented by project-specific code.
    }
};
//---------------------------------------------------------------------------------------------
class AIComponent : public ObjectComponent
{
public:
	explicit AIComponent() = default;
    virtual ~AIComponent() override = default;
    virtual ComponentType GetComponentType() const override { return ComponentType::AI; }
    virtual void SetOwner(Object* _owner) override;
    virtual void Process() override {/*AI logic goes here.*/ }

    float m_speed = 120.0f; // for tests to be removed
protected:
    float* fRef_posX;
    float* fRef_posY;
    float& m_posX = *fRef_posX;
    float& m_posY = *fRef_posY;

};
//---------------------------------------------------------------------------------------------
class VisualComponent : public ObjectComponent
{
public:
	explicit VisualComponent() = default;
    virtual ~VisualComponent() override = default;
    virtual ComponentType GetComponentType() const override { return ComponentType::Visual; }
    virtual void Render() override
    {
        // Drawing logic goes here.
    }
};
//---------------------------------------------------------------------------------------------
class AudioComponent : public ObjectComponent
{
public:
	explicit AudioComponent() = default;
    virtual ~AudioComponent() override = default;
    virtual ComponentType GetComponentType() const override { return ComponentType::Audio; }
    virtual void Process() override
    {
        // Audio update logic goes here.
    }
};
