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
#include <memory>
#include "system/system_utils.h"
#include "InputsManager.h"
#include "system/SystemMenu.h"
#include "VideoGame.h"
#include "system/Camera.h"

class GameEngine: public Singleton
{
	public:
		//GameEngine properties and methods
		GameEngine()
		{
			inputsmanager = std::make_unique<InputsManager>();
			systemmenu = std::make_unique<SystemMenu>();
			//videogame = sdt::make_unique<VideoGame>();
			VideoGame::GetInstance();

			name = "GameEngine";
			
			SYSTEM_LOG << "GameEngine created and Initialized\n";
		}
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
		// Per-class singleton accessors
		static GameEngine& GetInstance()
		{
			static GameEngine instance;
			return instance;
		}
		static GameEngine& Get() { return GetInstance(); }

		//-------------------------------------------------------------
		// Object Management
		std::vector<Object*> objectlist;

		void AddObject(Object* obj)
		{
			// Implementation to add object to the game engine
			objectlist.push_back(obj);
		}

		//-------------------------------------------------------------
	public:
		std::unique_ptr<InputsManager> inputsmanager;
		std::unique_ptr<SystemMenu> systemmenu;
		std::unique_ptr<VideoGame> videogame;
};