/*
Olympe Engine V2 2025
Nicolas Chereau
nchereau@gmail.com

Purpose:
- Class representing the core game engine
- Gamengine stores the list of all game objects

*/
#pragma once
#include "object.h"
#include <vector>
#include <memory>
#include "system/system_utils.h"
#include "InputsManager.h"
#include "system/SystemMenu.h"
#include "VideoGame.h"
#include "system/Camera.h"
#include "OptionsManager.h"

class VideoGame;
class EventManager;

class GameEngine: public Object
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

		virtual ObjectType GetObjectType() const { return ObjectType::Singleton; }

		//-------------------------------------------------------------
		// Per-class singleton accessors
		static GameEngine& GetInstance()
		{
			static GameEngine instance;
			return instance;
		}
		static GameEngine& Get() { return GetInstance(); }

public:
		EventManager* ptr_eventmanager = nullptr;
		InputsManager* ptr_inputsmanager = nullptr;
		SystemMenu* ptr_systemmenu = nullptr;
		VideoGame* ptr_videogame = nullptr;
		OptionsManager* ptr_optionsmanager = nullptr;

		EventManager& eventmanager = *ptr_eventmanager;
		InputsManager& inputsmanager = *ptr_inputsmanager;
		SystemMenu& systemmenu = *ptr_systemmenu;
		VideoGame& videogame = *ptr_videogame;
		OptionsManager& optionsmanager = *ptr_optionsmanager;

		void Initialize();

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



		static float fDt; // Delta Time between frames
		static int screenWidth;
		static int screenHeight;

		static SDL_Renderer* renderer;
		SDL_Renderer* GetMainRenderer() const {  return renderer; }
};