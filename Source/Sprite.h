#pragma once
#include "ObjectComponent.h"
#include <SDL3/SDL.h>

class Sprite : public VisualComponent
{
	public:
	explicit Sprite() = default;
	virtual ~Sprite() override = default;

	static bool FactoryRegistered;
	static ObjectComponent* Create();

	virtual void RenderDebug() override;
	virtual void Render() override;

protected:
	SDL_Texture* SpriteTexture = nullptr;
};

