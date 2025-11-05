#pragma once

#include <SDL3/SDL.h>
#include <unordered_map>
#include <mutex>
#include "message.h"
#include "system_consts.h"
#include "../object.h"

// Camera singleton that manages multiple per-player camera instances.
// Exposes minimal API used by the engine: Initialize(), Shutdown(), Apply(renderer)
// Additional helpers allow creating/removing cameras per player and query their transform.

class Camera : public Object
{
public:
    struct CameraInstance
    {
        short playerId = 0;
        float x = 0.0f; // top-left world coordinate
        float y = 0.0f;
        float zoom = 1.0f;
        SDL_Rect bounds{INT_MIN, INT_MIN, INT_MAX, INT_MAX};
        enum class Mode { Mode2D, Mode2_5D, ModeIsometric } mode = Mode::Mode2D;
        bool followTarget = false;
        int targetUid = -1; // optional follow target UID
        SDL_Point offset{0,0};
    };

public:
    Camera() { Initialize(); }
    ~Camera() { Shutdown(); }

    virtual ObjectType GetObjectType() const { return ObjectType::Singleton; }

    static Camera& GetInstance()
    {
        static Camera instance;
        return instance;
    }
    static Camera& Get() { return GetInstance(); }

    void Initialize();
    void Shutdown();

    // Backward-compatible single-apply: sets renderer viewport to first view and leaves it.
    void Apply(SDL_Renderer* renderer);

    // Utility: create/remove camera instance for a player
    void CreateCameraForPlayer(short playerID);
    void RemoveCameraForPlayer(short playerID);

    // Get camera instance for a player. If not present returns default camera (player 0).
    CameraInstance GetCameraForPlayer(short playerID) const;

    // Handle incoming engine messages for camera control
    void OnEvent(const Message& msg);

private:
    mutable std::mutex m_mutex;
    std::unordered_map<short, CameraInstance> m_cameras;
};
