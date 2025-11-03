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
			name = "GameEngine";
			SYSTEM_LOG << "GameEngine created and Initialized\n";
		}
		virtual ~GameEngine()	
		{
			SYSTEM_LOG << "GameEngine destroyed\n";
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
		void Process() override
		{
			// Calculate Delta Time for th entire engine cycle
			const Uint64 now = SDL_GetTicks();
			static Uint64 last_time = 0;
			fDt = ((float)(now - last_time)) / 1000.0f;  /* seconds since last iteration */
			last_time = now;

		}

		//-------------------------------------------------------------
	public:
		EventManager& eventmanager = EventManager::GetInstance();
		InputsManager& inputsmanager = InputsManager::GetInstance();
		SystemMenu& systemmenu = SystemMenu::GetInstance();
		VideoGame& videogame = VideoGame::GetInstance();

		static float fDt; // Delta Time between frames

		SDL_Renderer* renderer = NULL;

};