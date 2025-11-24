#include "Viewport.h"
#include "../GameEngine.h"

// Implementation file intentionally empty: all behavior implemented inline in header for simplicity.
// If later needed, complex logic can be moved here.

Viewport::Viewport() 
{ 
	Initialize(GameEngine::screenWidth, GameEngine::screenHeight); 
}

Viewport::~Viewport()
{
	Shutdown();
}
//-------------------------------------------------------------
void Viewport::Render()
{
    // For debug purposes, render viewport rectangles

    SDL_SetRenderDrawColor(GameEngine::renderer, 255, 0, 0, SDL_ALPHA_OPAQUE); // red
    for (const auto &rect : m_viewRects)
    {
		SDL_RenderRect(GameEngine::renderer, &rect);
	}
}
//-------------------------------------------------------------
void Viewport::Initialize(int w, int h)
{
    m_width = w; m_height = h;
    m_players.clear();
    m_playerIndexMap.clear();
    m_viewRects.clear();
    // default single viewport
    UpdateViewports();
    SYSTEM_LOG << "Viewport Initialized\n";
}
//-------------------------------------------------------------
void Viewport::Shutdown()
{
    m_players.clear();
    m_viewRects.clear();
    m_playerIndexMap.clear();
    SYSTEM_LOG << "Viewport Shutdown\n";
}
//-------------------------------------------------------------
// Add a player id (0..3). Returns true if added, false if already present or full
bool Viewport::AddPlayer(short playerID, ViewportLayout viewportLayout = ViewportLayout::ViewportLayout_Grid1x1)
{
    if (m_playerIndexMap.find(playerID) != m_playerIndexMap.end()) 
        return false;

    if (m_players.size() >= 4) 
        return false;

    m_players.push_back(playerID);
    m_playerIndexMap[playerID] = static_cast<int>(m_players.size() - 1);
    UpdateViewports();
    return true;
}
//-------------------------------------------------------------
bool Viewport::RemovePlayer(short playerID)
{
    auto it = std::find(m_players.begin(), m_players.end(), playerID);
    if (it == m_players.end()) return false;
    m_players.erase(it);
    m_playerIndexMap.clear();
    for (size_t i = 0; i < m_players.size(); ++i) m_playerIndexMap[m_players[i]] = static_cast<int>(i);
    UpdateViewports();
    return true;
}
//-------------------------------------------------------------
void Viewport::SetSize(int w, int h)
{
    m_width = w; m_height = h; UpdateViewports();
}
//-------------------------------------------------------------
// If a player exists, fill out outRect and return true
bool Viewport::GetViewRectForPlayer(short playerID, SDL_FRect& outRect) const
{
    auto it = m_playerIndexMap.find(playerID);
    if (it == m_playerIndexMap.end()) return false;
    int idx = it->second;
    if (idx < 0 || idx >= static_cast<int>(m_viewRects.size())) return false;
    outRect = m_viewRects[idx];
    return true;
}
//  -------------------------------------------------------------
void Viewport::UpdateViewports()
{
    m_viewRects.clear();
    int n = static_cast<int>(m_players.size());
    if (n == 0) n = 1; // default single view

    if (n == 1)
    {
        SDL_FRect r{ 0,0,m_width,m_height };
        m_viewRects.push_back(r);
    }
    else if (n == 2)
    {
        int w = m_width / 2;
        SDL_FRect r1{ 0,0,w,m_height };
        SDL_FRect r2{ w,0,m_width - w,m_height };
        m_viewRects.push_back(r1);
        m_viewRects.push_back(r2);
    }
    else if (n == 3)
    {
        int w = m_width / 3;
        SDL_FRect r1{ 0,0,w,m_height };
        SDL_FRect r2{ w,0,w,m_height };
        SDL_FRect r3{ 2 * w,0,m_width - 2 * w,m_height };
        m_viewRects.push_back(r1);
        m_viewRects.push_back(r2);
        m_viewRects.push_back(r3);
    }
    else // 4
    {
        int w = m_width / 2;
        int h = m_height / 2;
        SDL_FRect r1{ 0,0,w,h };
        SDL_FRect r2{ w,0,m_width - w,h };
        SDL_FRect r3{ 0,h,w,m_height - h };
        SDL_FRect r4{ w,h,m_width - w,m_height - h };
        m_viewRects.push_back(r1);
        m_viewRects.push_back(r2);
        m_viewRects.push_back(r3);
        m_viewRects.push_back(r4);
    }

    // if we have fewer players than rects (e.g. no players defined) we still keep rects sized to n
    // rebuild playerIndexMap if sizes mismatch
    m_playerIndexMap.clear();
    for (size_t i = 0; i < m_players.size() && i < m_viewRects.size(); ++i)
        m_playerIndexMap[m_players[i]] = static_cast<int>(i);
}