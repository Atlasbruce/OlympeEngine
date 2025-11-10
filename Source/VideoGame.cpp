/*
Purpose: Implementation of the VideoGame class, which represents a video game with attributes such as title, genre, and platform.
*/

#include "VideoGame.h"

VideoGame::VideoGame()
{
	name = "VideoGame";

	// Initialize viewport (default size)
	//Viewport::Get().Initialize(GameEngine::screenWidth, GameEngine::screenHeight);

	// Create default player 0
	//AddPlayer();

	// Register to EventManager for game events
	using EM = EventManager;
	EM::Get().Register(this, EventType::EventType_Game_Pause, [this](const Message& m){ this->OnEvent(m); });
	EM::Get().Register(this, EventType::EventType_Game_Resume, [this](const Message& m){ this->OnEvent(m); });
	EM::Get().Register(this, EventType::EventType_Game_Quit, [this](const Message& m){ this->OnEvent(m); });
	EM::Get().Register(this, EventType::EventType_Game_Restart, [this](const Message& m){ this->OnEvent(m); });
	EM::Get().Register(this, EventType::EventType_Game_AddPlayer, [this](const Message& m){ this->OnEvent(m); });
	EM::Get().Register(this, EventType::EventType_Game_RemovePlayer, [this](const Message& m){ this->OnEvent(m); });
	EM::Get().Register(this, EventType::EventType_Game_TakeScreenshot, [this](const Message& m){ this->OnEvent(m); });

	// Ensure default state is running
	GameStateManager::SetState(GameState::GameState_Running);
	testGao = (GameObject*)Factory::Get().CreateObject("Player");


	testGao = (GameObject*)Factory::Get().CreateObject("GameObject");
	testGao->name = "MorphingColor";
	testGao->position = { 400.0f, 300.0f };
	Factory::Get().AddComponent("ColorMorphComponent", testGao);

	SYSTEM_LOG << "VideoGame created\n";
}
//-------------------------------------------------------------
VideoGame::~VideoGame()
{
	SYSTEM_LOG << "VideoGame destroyed\n";
	// Unregister from EventManager
	EventManager::Get().UnregisterAll(this);
}
//-------------------------------------------------------------
// Player management: supports up to 4 players
// Returns assigned player ID [0..3] or -1 on failure
short VideoGame::AddPlayer()
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
        for (auto& kv : m_playerToJoystick) if (kv.second == id) { used = true; break; }
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
//-------------------------------------------------------------
bool VideoGame::RemovePlayer(const short PlayerID)
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
//-------------------------------------------------------------
// Event handler for EventManager messages registered in ctor
void VideoGame::OnEvent(const Message& msg)
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
                    SYSTEM_LOG << "VideoGame: Numpad - pressed -> remove viewport/player " << pid << " -> " << (ok ? "removed" : "failed") << "\n";
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