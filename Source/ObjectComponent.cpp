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

    // initialize internal position from owner if it's a GameObject
    if (auto gao = dynamic_cast<GameObject*>(owner))
    {
        fRef_posX = &gao->position.x;
        fRef_posY = &gao->position.y;
    }
}
