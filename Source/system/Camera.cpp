#include "Camera.h"
#include "EventManager.h"
#include "Viewport.h"
#include <iostream>

void Camera::Initialize()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // Create default camera for player 0
    CreateCameraForPlayer(0);

    // Register to camera-related events
    EventManager::Get().Register(this, EventType::EventType_Camera_Shake, [this](const Message& m){ this->OnEvent(m); });
    EventManager::Get().Register(this, EventType::EventType_Camera_Teleport, [this](const Message& m){ this->OnEvent(m); });
    EventManager::Get().Register(this, EventType::EventType_Camera_MoveToPosition, [this](const Message& m){ this->OnEvent(m); });
    EventManager::Get().Register(this, EventType::EventType_Camera_ZoomTo, [this](const Message& m){ this->OnEvent(m); });
    EventManager::Get().Register(this, EventType::EventType_Camera_Reset, [this](const Message& m){ this->OnEvent(m); });
    EventManager::Get().Register(this, EventType::EventType_Camera_Mode_SetBounds, [this](const Message& m){ this->OnEvent(m); });
    EventManager::Get().Register(this, EventType::EventType_Camera_Mode_2D, [this](const Message& m){ this->OnEvent(m); });
    EventManager::Get().Register(this, EventType::EventType_Camera_Mode_2_5D, [this](const Message& m){ this->OnEvent(m); });
    EventManager::Get().Register(this, EventType::EventType_Camera_Mode_Isometric, [this](const Message& m){ this->OnEvent(m); });
    EventManager::Get().Register(this, EventType::EventType_Camera_Target_Follow, [this](const Message& m){ this->OnEvent(m); });
    EventManager::Get().Register(this, EventType::EventType_Camera_Target_Unfollow, [this](const Message& m){ this->OnEvent(m); });
    EventManager::Get().Register(this, EventType::EventType_Camera_Target_SetOffset, [this](const Message& m){ this->OnEvent(m); });
    EventManager::Get().Register(this, EventType::EventType_Camera_Target_ClearOffset, [this](const Message& m){ this->OnEvent(m); });
    EventManager::Get().Register(this, EventType::EventType_Camera_Viewport_Add, [this](const Message& m){ this->OnEvent(m); });
    EventManager::Get().Register(this, EventType::EventType_Camera_Viewport_Remove, [this](const Message& m){ this->OnEvent(m); });
    EventManager::Get().Register(this, EventType::EventType_Camera_Viewport_Clear, [this](const Message& m){ this->OnEvent(m); });

	name = "Camera";

	SYSTEM_LOG << "Camera Initialized\n";
}

void Camera::Shutdown()
{
    // Unregister all callbacks associated with this instance
    EventManager::Get().UnregisterAll(this);
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cameras.clear();
	SYSTEM_LOG << "Camera Shutdown\n";
}

void Camera::CreateCameraForPlayer(short playerID)
{
    //std::lock_guard<std::mutex> lock(m_mutex);
    if (m_cameras.find(playerID) != m_cameras.end()) return;
    CameraInstance inst;
    inst.playerId = playerID;
    inst.x = 0.0f; inst.y = 0.0f; inst.zoom = 1.0f;
    m_cameras[playerID] = inst;
}

void Camera::RemoveCameraForPlayer(short playerID)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_cameras.find(playerID);
    if (it != m_cameras.end()) m_cameras.erase(it);
}

Camera::CameraInstance Camera::GetCameraForPlayer(short playerID) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_cameras.find(playerID);
    if (it != m_cameras.end()) return it->second;
    // fallback to player 0
    auto it0 = m_cameras.find(0);
    if (it0 != m_cameras.end()) return it0->second;
    // default instance
    CameraInstance d; d.playerId = 0; return d;
}

void Camera::Apply(SDL_Renderer* renderer)
{
    // Backwards-compatible: set render viewport to the first viewport rectangle
    if (!renderer) return;
    const auto& rects = Viewport::Get().GetViewRects();
    if (rects.size() > 0)
    {
        const SDL_Rect& r = rects[0];
        SDL_SetRenderViewport(renderer, &r);
    }
}

void Camera::OnEvent(const Message& msg)
{
    // Generic event handler that updates camera instances based on message payload.
    // Messages can target specific player via msg.deviceId (player index) or default to 0.
    short playerID = 0;
    if (msg.deviceId >= 0) playerID = static_cast<short>(msg.deviceId);

    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_cameras.find(playerID);
    if (it == m_cameras.end())
    {
        // if not present create it
        CreateCameraForPlayer(playerID);
        it = m_cameras.find(playerID);
    }
    CameraInstance& cam = it->second;

    switch (msg.type)
    {
        case EventType::EventType_Camera_Teleport:
        {
            // instant move camera to position (msg.value,msg.value2) or payload SDL_Point*
            if (msg.payload)
            {
                SDL_Point* p = reinterpret_cast<SDL_Point*>(msg.payload);
                cam.x = static_cast<float>(p->x);
                cam.y = static_cast<float>(p->y);
            }
            else
            {
                cam.x = msg.value;
                cam.y = msg.value2;
            }
            break;
        }
        case EventType::EventType_Camera_MoveToPosition:
        {
            // For now perform instant move (no blending) - could be extended
            cam.x = msg.value;
            cam.y = msg.value2;
            break;
        }
        case EventType::EventType_Camera_ZoomTo:
        {
            cam.zoom = msg.value;
            break;
        }
        case EventType::EventType_Camera_Reset:
        {
            cam.x = 0; cam.y = 0; cam.zoom = 1.0f; cam.offset = {0,0}; cam.followTarget = false; cam.targetUid = -1;
            break;
        }
        case EventType::EventType_Camera_Mode_SetBounds:
        {
            // bounds passed via payload pointer to SDL_Rect or via value/value2+controlId
            if (msg.payload)
            {
                SDL_Rect* r = reinterpret_cast<SDL_Rect*>(msg.payload);
                cam.bounds = *r;
            }
            else
            {
                // use controlId,width/height in value/value2
                cam.bounds.x = msg.controlId;
                cam.bounds.y = static_cast<int>(msg.value);
                cam.bounds.w = static_cast<int>(msg.value2);
                cam.bounds.h = 0; // unknown
            }
            break;
        }
        case EventType::EventType_Camera_Mode_2D:
            cam.mode = CameraInstance::Mode::Mode2D; break;
        case EventType::EventType_Camera_Mode_2_5D:
            cam.mode = CameraInstance::Mode::Mode2_5D; break;
        case EventType::EventType_Camera_Mode_Isometric:
            cam.mode = CameraInstance::Mode::ModeIsometric; break;
        case EventType::EventType_Camera_Target_Follow:
        {
            // follow target specified by msg.controlId as UID or payload pointer to Object*
            cam.followTarget = true;
            if (msg.controlId >= 0) cam.targetUid = msg.controlId;
            else if (msg.payload) cam.targetUid = *reinterpret_cast<int*>(msg.payload);
            break;
        }
        case EventType::EventType_Camera_Target_Unfollow:
            cam.followTarget = false; cam.targetUid = -1; break;
        case EventType::EventType_Camera_Target_SetOffset:
        {
            cam.offset.x = static_cast<int>(msg.value);
            cam.offset.y = static_cast<int>(msg.value2);
            break;
        }
        case EventType::EventType_Camera_Target_ClearOffset:
            cam.offset = {0,0}; break;
        case EventType::EventType_Camera_Viewport_Add:
        case EventType::EventType_Camera_Viewport_Remove:
        case EventType::EventType_Camera_Viewport_Clear:
            // viewport layout is handled by Viewport; nothing to do here by default
            break;
        default:
            break;
    }
}
