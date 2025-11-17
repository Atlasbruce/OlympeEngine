/*
Olympe Engine V2 2025
Nicolas Chereau
nchereau@gmail.com

Purpose:
- Class representing a Non-Player Character (NPC) in the game, derived from GameObject.

*/

#include "Npc.h"
#include "Factory.h"
bool Npc::FactoryRegistered = Factory::Get().Register("Npc", Npc::Create);
Object* Npc::Create()
{
	// Create a new Npc instance each time (not static)
	Npc* npc = new Npc();
	npc->name = "Npc";
	Factory::Get().AddComponent("AI_Npc", npc);
	return npc;
}