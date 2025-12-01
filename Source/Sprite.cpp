#include "Sprite.h"
#include "Factory.h"
#include "GameEngine.h"
#include "GameObject.h"
#include <SDL3/SDL_render.h>
#include "DataManager.h"

bool Sprite::FactoryRegistered = Factory::Get().Register("Sprite", Sprite::Create);
ObjectComponent* Sprite::Create()
{
	return new Sprite();
}
//-----------------------------------------------------
void Sprite::Initialize()
{

}

void Sprite::Uninitialize()
{
}

void Sprite::RenderDebug()
{
	/*
	SDL_SetRenderDrawColor(GameEngine::renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderRect(GameEngine::renderer, &gao->boundingBox);
	/**/
}

void Sprite::Render()
{
	Vector vRenderPos = gao->GetPosition() - CameraManager::Get().GetCameraPositionForPlayer();
	float _w, _h;
	gao->GetSize(_w, _h);
	gao->SetBoundingbox( {vRenderPos.x, vRenderPos.y, _w, _h} );

	if (m_SpriteTexture)
	{
		SDL_FRect box = gao->GetBoundingBox();

		SDL_RenderTexture(GameEngine::renderer, m_SpriteTexture, nullptr, &box);
	}
}

void Sprite::SetSprite(SDL_Texture* texture)
{
	m_SpriteTexture = texture;
	gao->SetSize((float)m_SpriteTexture->w, (float)m_SpriteTexture->h);
	//gao->width = gao->boundingBox.h = (float)m_SpriteTexture->h;
	//gao->height = gao->boundingBox.w = (float)m_SpriteTexture->w;
}

void Sprite::SetSprite(const std::string& resourceName,const std::string& filePath)
{
	SetSprite((SDL_Texture*)DataManager::Get().GetSprite(resourceName, filePath));
}

bool Sprite::Preload(const std::string& resourceName, const std::string& filePath)
{
	return DataManager::Get().PreloadSprite(resourceName, filePath, ResourceCategory::GameObject);
}
