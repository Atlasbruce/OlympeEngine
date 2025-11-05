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

//for tests
#include "GameObject.h"
#include "AI_Player.h"

static GameObject* testGao = nullptr;

class VideoGame : public Object
{
public:
    VideoGame()
    {
		name = "VideoGame";

		// Initialize components
		//world = std::make_unique<World>();
		//gamerules = std::make_unique<GameRules>("DefaultRules");
		//questmanager = std::make_unique<QuestManager>();
		//gamemenu = std::make_unique<GameMenu>();
		//camera = std::make_unique<Camera>();
		//viewport = std::make_unique<Viewport>();

		// Initialize viewport (default size)
		viewport.Initialize(800,600);

		// Create default player 0
		AddPlayer();

		// Register to EventManager for game events
		using EM = EventManager;
		EM::Get().Register(this, EventType::EventType_Game_Pause, [this](const Message& m){ this->OnEvent(m); });
		EM::Get().Register(this, EventType::EventType_Game_Resume, [this](const Message& m){ this->OnEvent(m); });
		EM::Get().Register(this, EventType::EventType_Game_Quit, [this](const Message& m){ this->OnEvent(m); });
		EM::Get().Register(this, EventType::EventType_Game_Restart, [this](const Message& m){ this->OnEvent(m); });
		EM::Get().Register(this, EventType::EventType_Game_AddPlayer, [this](const Message& m){ this->OnEvent(m); });
		EM::Get().Register(this, EventType::EventType_Game_RemovePlayer, [this](const Message& m){ this->OnEvent(m); });
		EM::Get().Register(this, EventType::EventType_Game_TakeScreenshot, [this](const Message& m){ this->OnEvent(m); });
		EM::Get().Register(this, EventType::EventType_Game_SaveState, [this](const Message& m){ this->OnEvent(m); });
		EM::Get().Register(this, EventType::EventType_Game_LoadState, [this](const Message& m){ this->OnEvent(m); });

		// Register to keyboard events to allow viewport add/remove with numpad +/-
		EM::Get().Register(this, EventType::EventType_Keyboard_KeyDown, [this](const Message& m){ this->OnEvent(m); });
		EM::Get().Register(this, EventType::EventType_Keyboard_KeyUp, [this](const Message& m){ this->OnEvent(m); });

		// Ensure default state is running
		GameStateManager::SetState(GameState::GameState_Running);

        testGao = (GameObject*)Factory::Get().CreateObject("Player");

		testGao = (GameObject*)Factory::Get().CreateObject("GameObject");
		testGao->name = "MorphingColor";
		testGao->position = { 400.0f, 300.0f };
		Factory::Get().AddComponent("ColorMorphComponent", testGao);

       SYSTEM_LOG << "VideoGame created\n";
	}

    ~VideoGame()
    {
        SYSTEM_LOG << "VideoGame destroyed\n";
        // Unregister from EventManager
        EventManager::Get().UnregisterAll(this);
    }

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
         viewport.AddPlayer(newId);
         camera.CreateCameraForPlayer(newId);
         SYSTEM_LOG << "VideoGame: Added player " << newId << "\n";
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
         SYSTEM_LOG << "VideoGame: Removed player " << PlayerID << "\n";
         return true;
     }

     int GetPlayerCount() const { return static_cast<int>(m_players.size()); }
     const std::vector<short>& GetPlayers() const { return m_players; }

     // Event handler for EventManager messages registered in ctor
     void OnEvent(const Message& msg)
     {
         switch (msg.type)
         {
             case EventType::EventType_Game_Pause:
                 Pause();
                 SYSTEM_LOG << "VideoGame: Paused via event\n";
                 break;
             case EventType::EventType_Game_Resume:
                 Resume();
                 SYSTEM_LOG << "VideoGame: Resumed via event\n";
                 break;
             case EventType::EventType_Game_Quit:
                 RequestQuit();
                 SYSTEM_LOG << "VideoGame: Quit requested via event\n";
                 break;
             case EventType::EventType_Game_Restart:
                 // Placeholder: restart current level (not implemented fully)
                 SYSTEM_LOG << "VideoGame: Restart requested via event (not implemented)\n";
                 break;
             case EventType::EventType_Game_AddPlayer:
             {
                 // controlId (if provided) may request a specific player id; otherwise add next
                 bool ok = false;
                 if (msg.controlId >= 0)
                 {
                     short requested = static_cast<short>(msg.controlId);
                     // if requested id already present, ignore
                     if (std::find(m_players.begin(), m_players.end(), requested) == m_players.end())
                     {
                         // attempt to add with that id by first creating players until needed id available
                         short added = AddPlayer();
                         // AddPlayer chooses lowest free id; if not the requested one, user can map controllers separately
                         (void)added;
                         ok = true;
                     }
                 }
                 else
                 {
                     short added = AddPlayer();
                     ok = (added >= 0);
                 }
                 SYSTEM_LOG << "VideoGame: AddPlayer event -> " << (ok ? "success" : "failed") << "\n";
                 break;
             }
             case EventType::EventType_Game_RemovePlayer:
             {
                 if (msg.controlId >= 0)
                 {
                     short pid = static_cast<short>(msg.controlId);
                     bool ok = RemovePlayer(pid);
                     SYSTEM_LOG << "VideoGame: RemovePlayer event for " << pid << " -> " << (ok ? "removed" : "not found") << "\n";
                 }
                 break;
             }
             case EventType::EventType_Game_TakeScreenshot:
                 // Not implemented: placeholder
                 SYSTEM_LOG << "VideoGame: TakeScreenshot event (not implemented)\n";
                 break;
             case EventType::EventType_Game_SaveState:
             {
                 int slot = msg.controlId;
                 SYSTEM_LOG << "VideoGame: SaveState event slot=" << slot << " (not implemented)\n";
                 break;
             }
             case EventType::EventType_Game_LoadState:
             {
                 int slot = msg.controlId;
                 SYSTEM_LOG << "VideoGame: LoadState event slot=" << slot << " (not implemented)\n";
                 break;
             }
             case EventType::EventType_Keyboard_KeyDown:
             {
                 // msg.controlId contains SDL_Scancode
                 auto sc = static_cast<SDL_Scancode>(msg.controlId);
                 if (sc == SDL_SCANCODE_KP_PLUS)
                 {
                     // debounce: only act on initial press
                     if (!m_kpPlusPressed && msg.state == 1)
                     {
                         m_kpPlusPressed = true;
                         short added = AddPlayer();
                         SYSTEM_LOG << "VideoGame: Numpad + pressed -> add viewport (AddPlayer returned " << added << ")\n";
                     }
                 }
                 else if (sc == SDL_SCANCODE_KP_MINUS)
                 {
                     if (!m_kpMinusPressed && msg.state == 1)
                     {
                         m_kpMinusPressed = true;
                         if (!m_players.empty())
                         {
                             short pid = m_players.back();
                             bool ok = RemovePlayer(pid);
                             SYSTEM_LOG << "VideoGame: Numpad - pressed -> remove viewport/player " << pid << " -> " << (ok?"removed":"failed") << "\n";
                         }
                     }
                 }
                 break;
             }
             case EventType::EventType_Keyboard_KeyUp:
             {
                 auto sc = static_cast<SDL_Scancode>(msg.controlId);
                 if (sc == SDL_SCANCODE_KP_PLUS) m_kpPlusPressed = false;
                 if (sc == SDL_SCANCODE_KP_MINUS) m_kpMinusPressed = false;
                 break;
             }
             default:
                 break;
         }
     }

public:
  //   std::unique_ptr<World> world;
  //   std::unique_ptr<GameRules> gamerules;
  //   std::unique_ptr<GameMenu> gamemenu;
  //   std::unique_ptr<QuestManager> questmanager;
	 //std::unique_ptr<Camera> camera;
	 //std::unique_ptr<Viewport> viewport;
	World& world = World::GetInstance();
	GameRules& gamerules = GameRules::GetInstance();
	GameMenu& gamemenu = GameMenu::GetInstance();
	QuestManager& questmanager = QuestManager::GetInstance();
	Camera& camera = Camera::GetInstance();
	Viewport& viewport = Viewport::GetInstance();

private:
     // cached state for quick local reads (authoritative value lives in GameStateManager)
     GameState m_state = GameState::GameState_Running;

     // Players
     std::vector<short> m_players;
     std::unordered_map<short, SDL_JoystickID> m_playerToJoystick; // -1 = keyboard
     bool m_keyboardAssigned = false;

     // key debounce flags for numpad +/-
     bool m_kpPlusPressed = false;
     bool m_kpMinusPressed = false;
};
