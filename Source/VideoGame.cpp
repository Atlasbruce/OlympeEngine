/*
Purpose: Implementation of the VideoGame class, which represents a video game with attributes such as title, genre, and platform.
*/

#include "VideoGame.h"
#include "DataManager.h"
#include "GameObject.h"

#include <sstream>
#include <string>

VideoGame::VideoGame()
{
	name = "VideoGame";

	// Initialize viewport (default size)
	/* deprecated called in the Viewport::Viewport
     Viewport::Get().Initialize(GameEngine::screenWidth, GameEngine::screenHeight);
    /**/

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

	// Ensure default state is running
	GameStateManager::SetState(GameState::GameState_Running);
	testGao = (GameObject*)Factory::Get().CreateObject("Player");


	testGao = (GameObject*)Factory::Get().CreateObject("GameObject");
	testGao->name = "OlympeSystem";
	//testGao->position = { 400.0f, 300.0f };
	Factory::Get().AddComponent("OlympeSystem", testGao);

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
        SYSTEM_LOG << "VideoGame: SaveState event slot=" << slot << "\n";
        SaveGame(slot);
        break;
    }
    case EventType::EventType_Game_LoadState:
    {
        int slot = msg.controlId;
        SYSTEM_LOG << "VideoGame: LoadState event slot=" << slot << "\n";
        LoadGame(slot);
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

// Save full game state into a single JSON file per slot using DataManager
// Optimized: pre-allocate string capacity to avoid reallocations
bool VideoGame::SaveGame(int slot) const
{
    std::string vgName = name.empty() ? std::string("DefaultGame") : name;
    std::ostringstream ss;
    
    // Estimate capacity: ~200 bytes overhead + ~300 bytes per object
    const auto& list = World::Get().GetObjectList();
    size_t estimatedSize = 200 + (list.size() * 300);
    ss.str(std::string());
    ss.str().reserve(estimatedSize);
    
    ss << "{\n";
    ss << "  \"videogame\": \"" << vgName << "\",\n";
    // players
    ss << "  \"players\": [";
    for (size_t i = 0; i < m_players.size(); ++i)
    {
        if (i) ss << ", "; ss << m_players[i];
    }
    ss << "],\n";

    // world objects
    ss << "  \"objects\": [\n";
    bool first = true;
    for (const auto* obj : list)
    {
        const GameObject* go = dynamic_cast<const GameObject*>(obj);
        if (!go) continue;
        if (!first) ss << ",\n";
        first = false;
        ss << go->Save();
    }
    ss << "\n  ]\n";
    ss << "}\n";

    std::string content = ss.str();
    std::string slotName = "SaveSlot_" + std::to_string(slot);
    bool ok = DataManager::Get().SaveJSONForObject(vgName, slotName, content);
    SYSTEM_LOG << "VideoGame: SaveGame -> " << (ok ? "success" : "failed") << " to slot '" << slotName << "'\n";
    return ok;
}

// Load game state from a slot file
bool VideoGame::LoadGame(int slot)
{
    std::string vgName = name.empty() ? std::string("DefaultGame") : name;
    std::string slotName = "SaveSlot_" + std::to_string(slot);
    std::string content;
    if (!DataManager::Get().LoadJSONForObject(vgName, slotName, content))
    {
        SYSTEM_LOG << "VideoGame: LoadGame failed to read slot '" << slotName << "'\n";
        return false;
    }

    // Optimized parser: extract objects array and re-create objects
    // Pre-reserve space to avoid reallocations
    size_t pos = content.find("\"objects\"");
    if (pos == std::string::npos) return false;
    size_t start = content.find('[', pos);
    if (start == std::string::npos) return false;
    size_t end = content.find(']', start);
    if (end == std::string::npos) return false;
    
    // Use string_view approach for parsing to avoid copies
    const char* arr_start = content.data() + start + 1;
    const char* arr_end = content.data() + end;
    
    // Count objects to reserve vector capacity
    std::vector<std::string> entries;
    size_t obj_count = 0;
    for (const char* p = arr_start; p < arr_end; ++p) {
        if (*p == '{') ++obj_count;
    }
    entries.reserve(obj_count);
    
    // Parse objects more efficiently
    std::string tmp(arr_start, arr_end - arr_start);
    size_t p = 0;
    while ((p = tmp.find("},{", p)) != std::string::npos) { 
        tmp.replace(p, 3, "}|{"); 
        p += 3; 
    }
    
    size_t cur = 0;
    while (cur < tmp.size()) {
        size_t sep = tmp.find('|', cur);
        if (sep == std::string::npos) sep = tmp.size();
        
        // Trim whitespace more efficiently
        size_t a = cur;
        while (a < sep && (tmp[a] == ' ' || tmp[a] == '\n' || tmp[a] == '\t' || tmp[a] == '\r')) ++a;
        size_t b = sep;
        while (b > a && (tmp[b-1] == ' ' || tmp[b-1] == '\n' || tmp[b-1] == '\t' || tmp[b-1] == '\r')) --b;
        
        if (b > a) {
            entries.emplace_back(tmp.substr(a, b - a));
        }
        cur = sep + 1;
    }

    // For simplicity: clear existing world objects (dangerous in real engine)
    auto &list = World::Get().GetObjectList();
    for (auto o : list) delete o;
    list.clear();

    for (auto &entry : entries)
    {
        // determine className
        std::string className = "GameObject";
        std::string extracted;
        // look for "className": "..."
        size_t cnpos = entry.find("\"className\"");
        if (cnpos != std::string::npos)
        {
            size_t colon = entry.find(':', cnpos);
            if (colon != std::string::npos)
            {
                size_t q1 = entry.find('"', colon);
                if (q1 != std::string::npos)
                {
                    size_t q2 = entry.find('"', q1+1);
                    if (q2 != std::string::npos && q2 > q1+1)
                    {
                        className = entry.substr(q1+1, q2 - q1 - 1);
                    }
                }
            }
        }

        Object* o = Factory::Get().CreateObject(className);
        GameObject* go = dynamic_cast<GameObject*>(o);
        if (go)
        {
            go->Load(entry);
        }
    }

    SYSTEM_LOG << "VideoGame: LoadGame completed for slot '" << slotName << "'\n";
    return true;
}