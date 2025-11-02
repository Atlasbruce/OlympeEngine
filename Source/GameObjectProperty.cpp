#include "GameObjectProperty.h"
#include "GameEngine.h"
#include "Factory.h"

bool GameObjectProperty::FactoryRegistered = Factory::Get().Register("GameObjectProperty", []() { return new GameObjectProperty(); });
float& GameObjectProperty::fDt = GameEngine::fDt;
