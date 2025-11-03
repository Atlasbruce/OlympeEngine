/*
Olympe Engine V2 2025
Nicolas Chereau
nchereau@gmail.com

Purpose:
- Class representing a player in the game, derived from GameObject.
- Handles Joypads and Input Mapping.
- Manages Events for collisions, interactions, and state changes.

*/

#include "Player.h"
#include "Factory.h"
bool Player::FactoryRegistered = Factory::Get().Register("Player", Player::Create);
Object* Player::Create()
{
	static Player *player = new Player();
	player->name = "Player";
	Factory::Get().AddComponent("AI_Player", player);
	Factory::Get().AddComponent("SpriteComponent", player);
	return player;
}