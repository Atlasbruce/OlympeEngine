#pragma once
#include "ObjectComponent.h"
#include <string>
#include <SDL3/SDL_surface.h>

class SpriteComponent : public VisualComponent
{
	public:
	explicit SpriteComponent() = default;
	virtual ~SpriteComponent() override = default;
	static bool FactoryRegistered;
	static ObjectComponent* Create();
	virtual void RenderDebug() override;
	virtual ComponentType GetComponentType() const override { return ComponentType::Visual; }
	virtual void Render() override;

protected:
	string SpritePath = "";
	SDL_Surface* Sprite = nullptr;
};

