#pragma once
#include "ObjectComponent.h"
class SpriteComponent : public VisualComponent
{
	public:
	explicit SpriteComponent() = default;
	virtual ~SpriteComponent() override = default;
	static bool FactoryRegistered;
	static ObjectComponent* Create();
	virtual ComponentType GetComponentType() const override { return ComponentType::Visual; }
	virtual void Render() override;
};

