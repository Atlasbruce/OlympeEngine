#pragma once

#include <SDL3/SDL.h>
#include <unordered_map>
#include <mutex>
#include "message.h"
#include "system_consts.h"
#include "../object.h"
#include "../vector.h"

// Camera singleton that manages multiple per-player camera instances.
// Exposes minimal API used by the engine: Initialize(), Shutdown(), Apply(renderer)
// Additional helpers allow creating/removing cameras per player and query their transform.

enum class CameraType
{
    CameraType_2D = 0,
    CameraType_2_5D,
    CameraType_Isometric,
	CameraType_Count
};

enum class CameraMode
{
    CameraMode_Standard_Fixed = 0, // camera fixed at position on main renderer
	CameraMode_Standard_Follow,    // camera follows an entity (player or other) on main renderer
	CameraMode_Viewport_Fixed,      // camera fixed at position on a viewport
	CameraMode_Viewport_Follow,     // camera follows an entity (player or other) on a viewport
    CameraMode_Count
};

class CameraManager : public Object
{
public:
    struct CameraInstance
    {
        short playerId = 0;
		Vector position;
        Vector offset;
        float zoom = 1.0f;
        SDL_Rect bounds{INT_MIN, INT_MIN, INT_MAX, INT_MAX};
        enum class Mode { Mode2D, Mode2_5D, ModeIsometric } mode = Mode::Mode2D;
        bool followTarget = false;
        int targetUid = -1; // optional follow target UID
        
		CameraMode camMode = CameraMode::CameraMode_Standard_Fixed;
		CameraType camType = CameraType::CameraType_2D;
    };

public:
    CameraManager() { Initialize(); }
    ~CameraManager() { Shutdown(); }

    virtual ObjectType GetObjectType() const { return ObjectType::Singleton; }

    static CameraManager& GetInstance()
    {
        static CameraManager instance;
        return instance;
    }
    static CameraManager& Get() { return GetInstance(); }

    void Initialize();
    void Shutdown();

    // Backward-compatible single-apply: sets renderer viewport to first view and leaves it.
    void Apply(SDL_Renderer* renderer);

    // Utility: create/remove camera instance for a player
    void CreateCameraForPlayer(short playerID);
    void RemoveCameraForPlayer(short playerID);

    // Get camera instance for a player. If not present returns default camera (player 0).
    CameraInstance GetCameraForPlayer(short playerID) const;

	void Process(); // per-frame processing (if needed)

    // Handle incoming engine messages for camera control
    void OnEvent(const Message& msg);

private:
    mutable std::mutex m_mutex;
    std::unordered_map<short, CameraInstance> m_cameraInstances;

};