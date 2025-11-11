#include "SpriteComponent.h"
#include "Factory.h"
#include "GameEngine.h"
#include "GameObject.h"
#include <SDL3/SDL_render.h>


bool SpriteComponent::FactoryRegistered = Factory::Get().Register("SpriteComponent", SpriteComponent::Create);
ObjectComponent* SpriteComponent::Create()
{
	return new SpriteComponent();
}

void SpriteComponent::RenderDebug()
{
	SDL_SetRenderDrawColor(GameEngine::renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);  /* red, full alpha */
	gao->boundingBox = { gao->position.x, gao->position.y, gao->width, gao->height };
	SDL_RenderRect(GameEngine::renderer, &gao->boundingBox);
}

void SpriteComponent::Render()
{

}
