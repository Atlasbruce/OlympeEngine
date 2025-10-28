/* VideoGame.h
 High-level videogame class that composes World, GameRules, QuestManager and GameMenu.
*/
#pragma once

#include "Singleton.h"
#include "World.h"
#include "GameRules.h"
#include "QuestManager.h"
#include "GameMenu.h"
#include <memory>
#include <iostream>

class VideoGame : public Singleton
{
public:
	VideoGame()
	{
		 std::cout << "VideoGame created\n";
		 // Initialize components
		 world = std::make_unique<World>("MainWorld");
		 rules = std::make_unique<GameRules>("DefaultRules");
		 // QuestManager is a singleton; ensure instance exists
		 QuestManager::Get();
		 menu = std::make_unique<GameMenu>();
	}
 virtual ~VideoGame() { std::cout << "VideoGame destroyed\n"; }

 static VideoGame& GetInstance()
 {
 static VideoGame instance;
 return instance;
 }
 static VideoGame& Get() { return GetInstance(); }

 World& GetWorld() { return *world; }
 GameRules& GetRules() { return *rules; }
 QuestManager& GetQuestManager() { return QuestManager::GetInstance(); }
 GameMenu& GetMenu() { return *menu; }

private:
 std::unique_ptr<World> world;
 std::unique_ptr<GameRules> rules;
 std::unique_ptr<GameMenu> menu;
};
