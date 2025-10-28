#pragma once

#include <atomic>

// Game state enum used by VideoGame and queried by World
enum class GameState
{
    GameState_Running = 0,
    GameState_Paused,
    GameState_Quit
};

namespace GameStateManager
{
    // Set the global game state
    void SetState(GameState s);

    // Get the global game state
    GameState GetState();

    // Helper
    inline bool IsPaused() { return GetState() == GameState::GameState_Paused; }
}
