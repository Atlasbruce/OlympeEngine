#include "Sprite.h"
#include "Factory.h"
#include "GameEngine.h"
#include "GameObject.h"
#include <SDL3/SDL_render.h>


bool Sprite::FactoryRegistered = Factory::Get().Register("Sprite", Sprite::Create);
ObjectComponent* Sprite::Create()
{
	return new Sprite();
}

void Sprite::RenderDebug()
{
	SDL_SetRenderDrawColor(GameEngine::renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);  /* red, full alpha */
	gao->boundingBox = { gao->position.x, gao->position.y, gao->width, gao->height };
	SDL_RenderRect(GameEngine::renderer, &gao->boundingBox);
}

void Sprite::Render()
{

}
