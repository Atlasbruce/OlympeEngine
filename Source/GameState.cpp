#include "GameState.h"

namespace {
    // single atomic storage for the state (int) - one definition in this TU
    static std::atomic<int> g_state(static_cast<int>(GameState::GameState_Running));
}

namespace GameStateManager
{
    void SetState(GameState s)
    {
        g_state.store(static_cast<int>(s));
    }

    GameState GetState()
    {
        return static_cast<GameState>(g_state.load());
    }
}
