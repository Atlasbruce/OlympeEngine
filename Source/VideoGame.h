/* VideoGame.h
 High-level videogame class that composes World, GameRules, QuestManager and GameMenu.
*/
#pragma once

#include "Singleton.h"
#include "World.h"
#include "GameRules.h"
#include "QuestManager.h"
#include "system/GameMenu.h"
#include "system/Camera.h"
#include "system/Viewport.h"
#include "system/JoystickManager.h"
#include "GameState.h"
#include <memory>
#include <iostream>
#include <vector>
#include <unordered_map>

class VideoGame : public Singleton
{
public:
	VideoGame()
	{
		 std::cout << "VideoGame created\n";
		 // Initialize components
		 world = std::make_unique<World>();
		 rules = std::make_unique<GameRules>("DefaultRules");
		 // QuestManager is a singleton; ensure instance exists
		 questmanager = std::make_unique<QuestManager>();
		 menu = std::make_unique<GameMenu>();

		 // Initialize camera singleton
		 Camera::Get().Initialize();

		 // Initialize viewport (default size)
		 Viewport::Get().Initialize(800,600);

		 // Create default player 0
		 AddPlayer();

		 // Ensure default state is running
		 GameStateManager::SetState(GameState::GameState_Running);
	}
 virtual ~VideoGame() { std::cout << "VideoGame destroyed\n"; }

 // Per-class singleton accessors
 static VideoGame& GetInstance()
 {
 static VideoGame instance;
 return instance;
 }
 static VideoGame& Get() { return GetInstance(); }

 World& GetWorld() { return *world; }
 GameRules& GetRules() { return *rules; }
 QuestManager& GetQuestManager() { return *questmanager; }
 GameMenu& GetMenu() { return *menu; }

 Camera& GetCamera() { return Camera::Get(); }

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
 short AddPlayer()
 {
     if (m_players.size() >= 4) return -1;
     // choose lowest unused id
     short newId = -1;
     for (short i = 0; i < 4; ++i) {
         if (std::find(m_players.begin(), m_players.end(), i) == m_players.end()) { newId = i; break; }
     }
     if (newId < 0) return -1;

     // assign a joystick if available (one joystick per player) otherwise use keyboard if not used
     auto joysticks = JoystickManager::Get().GetConnectedJoysticks();
     SDL_JoystickID picked = 0;
     bool assignedJoystick = false;
     for (auto id : joysticks)
     {
         bool used = false;
         for (auto &kv : m_playerToJoystick) if (kv.second == id) { used = true; break; }
         if (!used) { picked = id; assignedJoystick = true; break; }
     }

     if (assignedJoystick)
     {
         m_playerToJoystick[newId] = picked;
     }
     else
     {
         // if no joystick available, use keyboard only if keyboard not already used
         if (!m_keyboardAssigned)
         {
             m_keyboardAssigned = true;
             m_playerToJoystick[newId] = SDL_JoystickID(-1); // -1 denotes keyboard
         }
         else
         {
             // cannot add player: no free controller
             return -1;
         }
     }

     m_players.push_back(newId);
     // update viewport and create camera for player
     Viewport::Get().AddPlayer(newId);
     Camera::Get().CreateCameraForPlayer(newId);
     return newId;
 }

 bool RemovePlayer(const short PlayerID)
 {
     auto it = std::find(m_players.begin(), m_players.end(), PlayerID);
     if (it == m_players.end()) return false;
     // free controller mapping
     auto pit = m_playerToJoystick.find(PlayerID);
     if (pit != m_playerToJoystick.end())
     {
         if (pit->second == SDL_JoystickID(-1)) m_keyboardAssigned = false;
         m_playerToJoystick.erase(pit);
     }
     m_players.erase(it);
     Viewport::Get().RemovePlayer(PlayerID);
     Camera::Get().RemoveCameraForPlayer(PlayerID);
     return true;
 }

 int GetPlayerCount() const { return static_cast<int>(m_players.size()); }
 const std::vector<short>& GetPlayers() const { return m_players; }

private:
 std::unique_ptr<World> world;
 std::unique_ptr<GameRules> rules;
 std::unique_ptr<GameMenu> menu;
 std::unique_ptr<QuestManager> questmanager;

 // cached state for quick local reads (authoritative value lives in GameStateManager)
 GameState m_state = GameState::GameState_Running;

 // Players
 std::vector<short> m_players;
 std::unordered_map<short, SDL_JoystickID> m_playerToJoystick; // -1 = keyboard
 bool m_keyboardAssigned = false;
};
