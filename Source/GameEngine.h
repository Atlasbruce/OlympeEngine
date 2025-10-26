/*
Olympe Engine V2 2025
Nicolas Chereau
nchereau@gmail.com

Purpose:
- Class representing the core game engine
- Gamengine stores the list of all game objects

*/
#pragma once
#include "Singleton.h"
#include "Object.h"
#include <vector>

class GameEngine: public Singleton
{
	public:
		//GameEngine properties and methods
		GameEngine() = default;
		virtual ~GameEngine()
		{
			// Clean up all objects
			for (auto obj : objectlist)
			{
				delete obj;
			}
			objectlist.clear();
		}
		//-------------------------------------------------------------
		// Object Management
		std::vector<Object*> objectlist;

		void AddObject(Object* obj)
		{
			// Implementation to add object to the game engine
			objectlist.push_back(obj);
		}

		//-------------------------------------------------------------
};

