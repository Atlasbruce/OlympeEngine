/*
 Olympe Engine V2 2025
 Nicolas Chereau
 nchereau@gmail.com

 Purpose: 
 - CameraManager class to handle multiple camera instances for players

*/
#pragma once

#include <SDL3/SDL.h>
#include <unordered_map>
#include "message.h"
#include "../object.h"
#include "../vector.h"

class GameObject;


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
        float zoom = 1.0f;
        SDL_Rect bounds{INT_MIN, INT_MIN, INT_MAX, INT_MAX};
        bool followTarget = false;
		GameObject* targetObject = nullptr; // optional follow target pointer
		CameraMode mode = CameraMode::CameraMode_Standard_Fixed;
		CameraType type = CameraType::CameraType_2D;
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

	Vector GetCameraPositionForPlayer(short playerID = 0) const;

	void Process(); // per-frame processing (if needed)

    // Handle incoming engine messages for camera control
    void OnEvent(const Message& msg);

private:
    std::unordered_map<short, CameraInstance> m_cameraInstances;
};