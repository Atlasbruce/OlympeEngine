#include "CameraManager.h"
#include "EventManager.h"
#include "Viewport.h"
#include "../World.h"
#include "../vector.h"
#include "system_utils.h"
#include "../GameObject.h"
#include "../GameEngine.h"

using EM = EventManager;

void CameraManager::Initialize()
{
    // Create default camera for player 0
    CreateCameraForPlayer(0);

    // Register to camera-related events
    EM::Get().Register(this, EventType::Olympe_EventType_Camera_Shake);
    EM::Get().Register(this, EventType::Olympe_EventType_Camera_Teleport);
    EM::Get().Register(this, EventType::Olympe_EventType_Camera_MoveToPosition);
    EM::Get().Register(this, EventType::Olympe_EventType_Camera_ZoomTo);
    EM::Get().Register(this, EventType::Olympe_EventType_Camera_Reset);
    EM::Get().Register(this, EventType::Olympe_EventType_Camera_Mode_2D);
    EM::Get().Register(this, EventType::Olympe_EventType_Camera_Mode_2_5D);
    EM::Get().Register(this, EventType::Olympe_EventType_Camera_Mode_Isometric);
    EM::Get().Register(this, EventType::Olympe_EventType_Camera_Target_Follow);
    EM::Get().Register(this, EventType::Olympe_EventType_Camera_Target_Unfollow);
    EM::Get().Register(this, EventType::Olympe_EventType_Camera_Viewport_Add);
    EM::Get().Register(this, EventType::Olympe_EventType_Camera_Viewport_Remove);
    EM::Get().Register(this, EventType::Olympe_EventType_Camera_Viewport_Clear);

	SYSTEM_LOG << "CameraManager Initialized\n";
}

void CameraManager::Shutdown()
{
    // Unregister all callbacks associated with this instance
    EventManager::Get().UnregisterAll(this);
    m_cameraInstances.clear();
	SYSTEM_LOG << "Camera Shutdown\n";
}

void CameraManager::CreateCameraForPlayer(short playerID)
{
    if (m_cameraInstances.find(playerID) != m_cameraInstances.end()) return;
    CameraInstance inst;
    inst.playerId = playerID;
	// set camera offset to screen center by default
	inst.offset = { -GameEngine::screenWidth / 2.f, -GameEngine::screenHeight / 2.f, 0.0f };
    inst.zoom = 1.0f;
    m_cameraInstances[playerID] = inst;
}

void CameraManager::RemoveCameraForPlayer(short playerID)
{
    auto it = m_cameraInstances.find(playerID);
    if (it != m_cameraInstances.end()) m_cameraInstances.erase(it);
}

CameraManager::CameraInstance CameraManager::GetCameraForPlayer(short playerID = 0) const
{
    if (m_cameraInstances.empty()) 
    {
        CameraInstance d; 
        d.playerId = 0; 
        return d;
	}

    auto it = m_cameraInstances.find(playerID);

    if (it != m_cameraInstances.end()) 
        return it->second;
    // fallback to player 0
    auto it0 = m_cameraInstances.find(0);
    if (it0 != m_cameraInstances.end()) 
        return it0->second;
}

Vector CameraManager::GetCameraPositionForPlayer(short playerID) const
{
	if (m_cameraInstances.empty()) return Vector();

	auto it = m_cameraInstances.find(playerID);
	if (it != m_cameraInstances.end()) return it->second.position;
	// fallback to player 0
	auto it0 = m_cameraInstances.find(0);
	if (it0 != m_cameraInstances.end()) return it0->second.position;
}

void CameraManager::Process()
{
	//process only if there are camera instances
	if (m_cameraInstances.empty()) return;
    
    // For each camera instance, update position if following target, apply bounds, etc.
    for (auto& kv : m_cameraInstances)
    {
        CameraInstance& cam = kv.second;
        if (cam.followTarget && cam.targetObject)
        {
            // Simple follow logic (could be smoothed)
			Vector v2 = cam.targetObject->position + cam.offset;
            cam.position = vBlend(cam.position, v2 , 0.75f);
		}
    }
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

            break;
        }
        case EventType::Olympe_EventType_Camera_MoveToPosition:
        {
            // For now perform instant move (no blending) - could be extended
            cam.position.x = msg.param1;
            cam.position.y = msg.param2;
            break;
        }
        case EventType::Olympe_EventType_Camera_ZoomTo:
        {
            cam.zoom = msg.param1;
            break;
        }
        case EventType::Olympe_EventType_Camera_Reset:
        {
            cam.position.x = 0; cam.position.y = 0; cam.zoom = 1.0f; cam.followTarget = false; cam.targetObject = nullptr;
            break;
        }
        case EventType::Olympe_EventType_Camera_Mode_2D:
            cam.type = CameraType::CameraType_2D;
            break;
        case EventType::Olympe_EventType_Camera_Mode_2_5D:
            cam.type = CameraType::CameraType_2_5D;
            break;
        case EventType::Olympe_EventType_Camera_Mode_Isometric:
            cam.type = CameraType::CameraType_Isometric;
            break;
        case EventType::Olympe_EventType_Camera_Target_Follow:
        {
            // follow target specified by msg.controlId as UID or payload pointer to Object*
            cam.followTarget = true;
			cam.targetObject = (GameObject*)msg.objectParamPtr;
            if (cam.targetObject == nullptr)
            {
                // try to get object by UID
                cam.targetObject = (GameObject*)World::Get().GetObjectByUID(msg.targetUid);
			}
            break;
        }
        case EventType::Olympe_EventType_Camera_Target_Unfollow:
        {
            cam.followTarget = false; cam.targetObject = nullptr;
            break;
        }
        case EventType::Olympe_EventType_Camera_Viewport_Add:
        {
            // viewport layout is handled by Viewport; nothing to do here by default
            break;
        }
        case EventType::Olympe_EventType_Camera_Viewport_Remove:
            break;
        case EventType::Olympe_EventType_Camera_Viewport_Clear:
            break;
        default:
            break;
    }
}