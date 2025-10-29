#include "JoystickManager.h"
#include "EventManager.h"
#include "system_utils.h"
#include <SDL3/SDL.h>
#include <iostream>

//---------------------------------------------------------------------------------------------
JoystickManager& JoystickManager::GetInstance()
{
    static JoystickManager instance;
    return instance;
}
//---------------------------------------------------------------------------------------------
void JoystickManager::Initialize()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // Enable joystick events so SDL will post them to the event queue
    SDL_SetJoystickEventsEnabled(true);

    int count =0;
    SDL_JoystickID* ids = SDL_GetJoysticks(&count);
    if (ids)
    {
        for (int i =0; i < count; ++i)
        {
            OpenJoystick(ids[i]);
        }
        SDL_free(ids);
    }
	SYSTEM_LOG << "JoystickManager created and Initialized with " << count << " joysticks connected\n";
}
//---------------------------------------------------------------------------------------------
void JoystickManager::Shutdown()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto &kv : m_joysticks)
    {
        if (kv.second.joystick)
        {
            SDL_CloseJoystick(kv.second.joystick);
        }
    }
    m_joysticks.clear();
}
//---------------------------------------------------------------------------------------------
void JoystickManager::Process(float /*dt*/)
{
    // optional polling-based updates could be implemented here
    // For now, joystick activity is driven by SDL events handled in HandleEvent().
}
//---------------------------------------------------------------------------------------------
void JoystickManager::HandleEvent(const SDL_Event* ev)
{
    if (!ev) return;

    switch (ev->type)
    {
        case SDL_EVENT_JOYSTICK_ADDED:
        {
            SDL_JoystickID id = ev->jdevice.which;
            OpenJoystick(id);
            SYSTEM_LOG << "Joystick added id=" << id << "\n";
            break;
        }
        case SDL_EVENT_JOYSTICK_REMOVED:
        {
            SDL_JoystickID id = ev->jdevice.which;
            CloseJoystick(id);
            SYSTEM_LOG << "Joystick removed id=" << id << "\n";
            break;
        }
        case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
        {
            SDL_JoystickID which = ev->jbutton.which;
            int button = ev->jbutton.button;
            bool down = ev->jbutton.down;
            PostJoystickButtonEvent(which, button, down);
            break;
        }
        case SDL_EVENT_JOYSTICK_BUTTON_UP:
        {
            SDL_JoystickID which = ev->jbutton.which;
            int button = ev->jbutton.button;
            bool down = ev->jbutton.down;
            PostJoystickButtonEvent(which, button, down);
            break;
        }
        case SDL_EVENT_JOYSTICK_AXIS_MOTION:
        {
            SDL_JoystickID which = ev->jaxis.which;
            int axis = ev->jaxis.axis;
            Sint16 value = ev->jaxis.value;
            PostJoystickAxisEvent(which, axis, value);
            break;
        }
        default:
            break;
    }
}
//---------------------------------------------------------------------------------------------
std::vector<SDL_JoystickID> JoystickManager::GetConnectedJoysticks()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<SDL_JoystickID> res;
    res.reserve(m_joysticks.size());
    for (auto &kv : m_joysticks) res.push_back(kv.first);
    return res;
}
//---------------------------------------------------------------------------------------------
bool JoystickManager::IsJoystickConnected(SDL_JoystickID id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_joysticks.find(id) != m_joysticks.end();
}
//---------------------------------------------------------------------------------------------
void JoystickManager::OpenJoystick(SDL_JoystickID instance_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_joysticks.find(instance_id) != m_joysticks.end()) return; // already open

    SDL_Joystick* js = SDL_OpenJoystick(instance_id);
    if (!js)
    {
        SYSTEM_LOG << "Failed to open joystick " << instance_id << " : " << SDL_GetError() << "\n";
        return;
    }

    JoystickInfo info;
    info.joystick = js;
    info.id = SDL_GetJoystickID(js);
    const char* name = SDL_GetJoystickName(js);
    info.name = name ? name : "<unknown>";
    info.numAxes = SDL_GetNumJoystickAxes(js);
    info.numButtons = SDL_GetNumJoystickButtons(js);
    info.axes.assign(info.numAxes,0);
    info.buttons.assign(info.numButtons, false);

    // read initial states
    for (int a =0; a < info.numAxes; ++a)
        info.axes[a] = SDL_GetJoystickAxis(js, a);
    for (int b =0; b < info.numButtons; ++b)
        info.buttons[b] = SDL_GetJoystickButton(js, b);

    m_joysticks[info.id] = std::move(info);

    // Optionally notify other systems about connection (not required by spec)
}
//---------------------------------------------------------------------------------------------
void JoystickManager::CloseJoystick(SDL_JoystickID instance_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_joysticks.find(instance_id);
    if (it == m_joysticks.end()) return;

    if (it->second.joystick)
    {
        SDL_CloseJoystick(it->second.joystick);
    }
    m_joysticks.erase(it);
}
//---------------------------------------------------------------------------------------------
void JoystickManager::PostJoystickButtonEvent(SDL_JoystickID which, int button, bool down)
{
    Message msg(down ? EventType::EventType_Joystick_ButtonDown : EventType::EventType_Joystick_ButtonUp, this);
    msg.deviceId = static_cast<int>(which);
    msg.controlId = button;
    msg.state = down ?1 :0;
    msg.value = down ?1.0f :0.0f;

    EventManager::Get().PostMessage(msg);
}
//---------------------------------------------------------------------------------------------
void JoystickManager::PostJoystickAxisEvent(SDL_JoystickID which, int axis, Sint16 value)
{
    Message msg(EventType::EventType_Joystick_AxisMotion, this);
    msg.deviceId = static_cast<int>(which);
    msg.controlId = axis;
    msg.state =0;
    // normalize Sint16 (-32768..32767) to [-1.0,1.0]
    float normalized =0.0f;
    if (value >=0) normalized = (value /32767.0f);
    else normalized = (value /32768.0f);
    msg.value = normalized;

    EventManager::Get().PostMessage(msg);
}
