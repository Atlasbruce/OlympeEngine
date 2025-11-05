#include "GameObject.h"
#include "Factory.h"
//for tests
//#include <SDL3/SDL_render.h>
#include "GameEngine.h"
//extern SDL_Renderer* renderer;

bool GameObject::FactoryRegistered = Factory::Get().Register("GameObject", Create); // []() { return new GameObject(); });

Object* GameObject::Create()
{
	return new GameObject();
}
