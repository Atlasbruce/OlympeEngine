#include "ObjectComponent.h"
#include "GameEngine.h"
#include "GameObject.h"

float& ObjectComponent::fDt = GameEngine::fDt;

void AIComponent::SetOwner(Object* _owner)
{
    ObjectComponent::SetOwner(_owner);
    if (!_owner)
    {
        SYSTEM_LOG << "Error AIComponent::SetOwner called with null owner!\n";
        return;
    }
    if (owner->GetObjectType() != ObjectType::Entity)
    {
        SYSTEM_LOG << "Error AIComponent::SetOwner called with non-Entity owner!\n";
        return;
    }
    
    fRef_posX = &(gao->position.x);
    fRef_posY = &(gao->position.y);
}
//-------------------------------------------------------------
void ObjectComponent::OnEvent(const Message& msg)
{
    if (msg.struct_type == EventStructType::EventStructType_System_Windows)
    {
        SYSTEM_LOG << "Warning ObjectComponent::OnEvent: received Windows event for GameObject: " << name << endl;
        return;
    }
}
//-------------------------------------------------------------
void PhysicsComponent::OnEvent(const Message& msg)
{
    ObjectComponent::OnEvent(msg);
}
//-------------------------------------------------------------
void AIComponent::OnEvent(const Message& msg)
{
    ObjectComponent::OnEvent(msg);
}
//-------------------------------------------------------------
void VisualComponent::OnEvent(const Message& msg)
{
    ObjectComponent::OnEvent(msg);
}
//-------------------------------------------------------------
void AudioComponent::OnEvent(const Message& msg)
{
}
