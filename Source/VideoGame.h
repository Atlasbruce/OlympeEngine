/* VideoGame.h
 High-level videogame class that composes World, GameRules, QuestManager and GameMenu.
*/
#pragma once

#include "object.h"
#include "World.h"
#include "GameRules.h"
#include "QuestManager.h"
#include "system/GameMenu.h"
#include "system/Camera.h"
#include "system/Viewport.h"
#include "system/JoystickManager.h"
#include "system/EventManager.h"
#include "system/message.h"
#include "GameState.h"
#include <memory>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "system/system_utils.h"
#include "Factory.h"
#include "GameEngine.h"

//for tests
#include "GameObject.h"
#include "AI_Player.h"

static GameObject* testGao = nullptr;

class VideoGame : public Object
{
public:
    VideoGame();

    virtual ~VideoGame();

    virtual ObjectType GetObjectType() const { return ObjectType::Singleton; }

    // Per-class singleton accessors
    static VideoGame& GetInstance()
    {
       static VideoGame instance;
       return instance;
    }
    static VideoGame& Get() { return GetInstance(); }
    inline World& GetWorld() { return world; }
    inline GameRules& GetRules() { return gamerules; }
    inline QuestManager& GetQuestManager() { return questmanager; }
    inline GameMenu& GetMenu() { return gamemenu; }
    inline Camera& GetCamera() { return camera; }
	inline Viewport& GetViewport() { return viewport; }

     // Game state helpers (front-end to GameStateManager)
     void SetState(GameState s)
     {
         GameStateManager::SetState(s);
         m_state = s;
     }
     GameState GetState() const { return GameStateManager::GetState(); }
     bool IsPaused() const { return GameStateManager::IsPaused(); }

     void Pause() { SetState(GameState::GameState_Paused); }
     void Resume() { SetState(GameState::GameState_Running); }
     void RequestQuit() { SetState(GameState::GameState_Quit); }

     // Player management: supports up to 4 players
     // Returns assigned player ID [0..3] or -1 on failure
     short AddPlayer();

     bool RemovePlayer(const short PlayerID);

     // Event handler for EventManager messages registered in ctor
     void OnEvent(const Message& msg);

     // Save / Load game state (slot optional)
     bool SaveGame(int slot = 0) const;
     bool LoadGame(int slot = 0);

public:
	World& world = World::GetInstance();
	GameRules& gamerules = GameRules::GetInstance();
	GameMenu& gamemenu = GameMenu::GetInstance();
	QuestManager& questmanager = QuestManager::GetInstance();
	Camera& camera = Camera::GetInstance();
	Viewport& viewport = Viewport::GetInstance();

    // Players
    std::vector<GameObject*> m_players;
    static short m_playerIdCounter;

private:
     // cached state for quick local reads (authoritative value lives in GameStateManager)
     GameState m_state = GameState::GameState_Running;

     bool m_keyboardAssigned = false;

     // key debounce flags for numpad +/-
     bool m_kpPlusPressed = false;
     bool m_kpMinusPressed = false;
};
