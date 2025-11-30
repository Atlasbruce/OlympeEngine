#include "CameraManager.h"
#include "EventManager.h"
#include "Viewport.h"
#include <iostream>

void CameraManager::Initialize()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // Create default camera for player 0
    CreateCameraForPlayer(0);

    // Register to camera-related events
    EventManager::Get().Register(this, EventType::Olympe_EventType_Camera_Shake);
    EventManager::Get().Register(this, EventType::Olympe_EventType_Camera_Teleport);
    EventManager::Get().Register(this, EventType::Olympe_EventType_Camera_MoveToPosition);
    EventManager::Get().Register(this, EventType::Olympe_EventType_Camera_ZoomTo);
    EventManager::Get().Register(this, EventType::Olympe_EventType_Camera_Reset);
    EventManager::Get().Register(this, EventType::Olympe_EventType_Camera_Mode_SetBounds);
    EventManager::Get().Register(this, EventType::Olympe_EventType_Camera_Mode_2D);
    EventManager::Get().Register(this, EventType::Olympe_EventType_Camera_Mode_2_5D);
    EventManager::Get().Register(this, EventType::Olympe_EventType_Camera_Mode_Isometric);
    EventManager::Get().Register(this, EventType::Olympe_EventType_Camera_Target_Follow);
    EventManager::Get().Register(this, EventType::Olympe_EventType_Camera_Target_Unfollow);
    EventManager::Get().Register(this, EventType::Olympe_EventType_Camera_Target_SetOffset);
    EventManager::Get().Register(this, EventType::Olympe_EventType_Camera_Target_ClearOffset);
    EventManager::Get().Register(this, EventType::Olympe_EventType_Camera_Viewport_Add);
    EventManager::Get().Register(this, EventType::Olympe_EventType_Camera_Viewport_Remove);
    EventManager::Get().Register(this, EventType::Olympe_EventType_Camera_Viewport_Clear);

	name = "CameraManager";

	SYSTEM_LOG << "CameraManager Initialized\n";
}

void CameraManager::Shutdown()
{
    // Unregister all callbacks associated with this instance
    EventManager::Get().UnregisterAll(this);
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cameraInstances.clear();
	SYSTEM_LOG << "Camera Shutdown\n";
}

void CameraManager::CreateCameraForPlayer(short playerID)
{
    //std::lock_guard<std::mutex> lock(m_mutex);
    if (m_cameraInstances.find(playerID) != m_cameraInstances.end()) return;
    CameraInstance inst;
    inst.playerId = playerID;
    inst.x = 0.0f; inst.y = 0.0f; inst.zoom = 1.0f;
    m_cameraInstances[playerID] = inst;
}

void CameraManager::RemoveCameraForPlayer(short playerID)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_cameraInstances.find(playerID);
    if (it != m_cameraInstances.end()) m_cameraInstances.erase(it);
}

CameraManager::CameraInstance CameraManager::GetCameraForPlayer(short playerID) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_cameraInstances.find(playerID);
    if (it != m_cameraInstances.end()) return it->second;
    // fallback to player 0
    auto it0 = m_cameraInstances.find(0);
    if (it0 != m_cameraInstances.end()) return it0->second;
    // default instance
    CameraInstance d; d.playerId = 0; return d;
}

void CameraManager::Apply(SDL_Renderer* renderer)
{
    // Backwards-compatible: set render viewport to the first viewport rectangle
    if (!renderer) return;
    const auto& rects = Viewport::Get().GetViewRects();
    if (rects.size() > 0)
    {
		const SDL_Rect r = { (int)rects[0].x, (int)rects[0].y, (int)rects[0].w, (int)rects[0].h };
        SDL_SetRenderViewport(renderer, &r);
    }
}

void CameraManager::OnEvent(const Message& msg)
{
    // Generic event handler that updates camera instances based on message payload.
    // Messages can target specific player via msg.deviceId (player index) or default to 0.
    short playerID = 0;
    if (msg.deviceId >= 0) playerID = static_cast<short>(msg.deviceId);

    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_cameraInstances.find(playerID);
    if (it == m_cameraInstances.end())
    {
        // if not present create it
        CreateCameraForPlayer(playerID);
        it = m_cameraInstances.find(playerID);
    }
    CameraInstance& cam = it->second;

    switch (msg.msg_type)
    {
        case EventType::Olympe_EventType_Camera_Teleport:
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
        case EventType::Olympe_EventType_Camera_MoveToPosition:
        {
            // For now perform instant move (no blending) - could be extended
            cam.x = msg.value;
            cam.y = msg.value2;
            break;
        }
        case EventType::Olympe_EventType_Camera_ZoomTo:
        {
            cam.zoom = msg.value;
            break;
        }
        case EventType::Olympe_EventType_Camera_Reset:
        {
            cam.x = 0; cam.y = 0; cam.zoom = 1.0f; cam.offset = {0,0}; cam.followTarget = false; cam.targetUid = -1;
            break;
        }
        case EventType::Olympe_EventType_Camera_Mode_SetBounds:
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
        case EventType::Olympe_EventType_Camera_Mode_2D:
            cam.mode = CameraInstance::Mode::Mode2D;
            break;
        case EventType::Olympe_EventType_Camera_Mode_2_5D:
            cam.mode = CameraInstance::Mode::Mode2_5D;
            break;
        case EventType::Olympe_EventType_Camera_Mode_Isometric:
            cam.mode = CameraInstance::Mode::ModeIsometric;
            break;
        case EventType::Olympe_EventType_Camera_Target_Follow:
        {
            // follow target specified by msg.controlId as UID or payload pointer to Object*
            cam.followTarget = true;
            if (msg.controlId >= 0) cam.targetUid = msg.controlId;
            else if (msg.payload) cam.targetUid = *reinterpret_cast<int*>(msg.payload);
            break;
        }
        case EventType::Olympe_EventType_Camera_Target_Unfollow:
            cam.followTarget = false; cam.targetUid = -1; break;
        case EventType::Olympe_EventType_Camera_Target_SetOffset:
        {
            cam.offset.x = msg.value;
            cam.offset.y = msg.value2;
            break;
        }
        case EventType::Olympe_EventType_Camera_Target_ClearOffset:
            cam.offset = {0.f,0.f};
            break;
        case EventType::Olympe_EventType_Camera_Viewport_Remove:
            break;
        case EventType::Olympe_EventType_Camera_Viewport_Clear:
			break;
        case EventType::Olympe_EventType_Camera_Viewport_Add:
        {
            // viewport layout is handled by Viewport; nothing to do here by default
            break;
        }
        default:
            break;
    }
}