#pragma once
#include "../object.h"
#include <SDL3/SDL.h>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "system_utils.h"

// Simple Viewport manager singleton that computes up to 4 view rectangles
// according to the number of players. Viewports are laid out as follows:
// 1 player: full screen
// 2 players: vertical split (2 columns)
// 3 players: vertical split (3 columns)
// 4 players: 2x2 grid

class Viewport : public Object
{
public:
    Viewport() {}// { Initialize(); }
	~Viewport() { Shutdown (); }

    virtual ObjectType GetObjectType() const { return ObjectType::Singleton; }

    static Viewport& GetInstance()
    {
        static Viewport instance;
        return instance;
    }
    static Viewport& Get() { return GetInstance(); }

    void Initialize(int w = 800, int h = 600)
    {
        m_width = w; m_height = h;
        m_players.clear();
        m_playerIndexMap.clear();
        m_viewRects.clear();
        // default single viewport
        UpdateViewports();
		SYSTEM_LOG << "Viewport Initialized\n";
    }

    void Shutdown()
    {
        m_players.clear();
        m_viewRects.clear();
        m_playerIndexMap.clear();
		SYSTEM_LOG << "Viewport Shutdown\n";
    }

    // Add a player id (0..3). Returns true if added, false if already present or full
    bool AddPlayer(short playerID)
    {
        if (m_playerIndexMap.find(playerID) != m_playerIndexMap.end()) return false;
        if (m_players.size() >= 4) return false;
        m_players.push_back(playerID);
        m_playerIndexMap[playerID] = static_cast<int>(m_players.size() - 1);
        UpdateViewports();
        return true;
    }

    bool RemovePlayer(short playerID)
    {
        auto it = std::find(m_players.begin(), m_players.end(), playerID);
        if (it == m_players.end()) return false;
        m_players.erase(it);
        m_playerIndexMap.clear();
        for (size_t i = 0; i < m_players.size(); ++i) m_playerIndexMap[m_players[i]] = static_cast<int>(i);
        UpdateViewports();
        return true;
    }

    void SetSize(int w, int h)
    {
        m_width = w; m_height = h; UpdateViewports();
    }

    int GetPlayerCount() const { return static_cast<int>(m_players.size() == 0 ? 1 : m_players.size()); }

    // Return view rects in the same order as m_players (if no players defined returns single full-screen rect)
    const std::vector<SDL_Rect>& GetViewRects() const { return m_viewRects; }

    // If a player exists, fill out outRect and return true
    bool GetViewRectForPlayer(short playerID, SDL_Rect& outRect) const
    {
        auto it = m_playerIndexMap.find(playerID);
        if (it == m_playerIndexMap.end()) return false;
        int idx = it->second;
        if (idx < 0 || idx >= static_cast<int>(m_viewRects.size())) return false;
        outRect = m_viewRects[idx];
        return true;
    }

    // Return ordered list of player ids (if empty, engine should treat it as single default player)
    const std::vector<short>& GetPlayers() const { return m_players; }

    void UpdateViewports()
    {
        m_viewRects.clear();
        int n = static_cast<int>(m_players.size());
        if (n == 0) n = 1; // default single view

        if (n == 1)
        {
            SDL_Rect r{0,0,m_width,m_height};
            m_viewRects.push_back(r);
        }
        else if (n == 2)
        {
            int w = m_width / 2;
            SDL_Rect r1{0,0,w,m_height};
            SDL_Rect r2{w,0,m_width - w,m_height};
            m_viewRects.push_back(r1);
            m_viewRects.push_back(r2);
        }
        else if (n == 3)
        {
            int w = m_width / 3;
            SDL_Rect r1{0,0,w,m_height};
            SDL_Rect r2{w,0,w,m_height};
            SDL_Rect r3{2*w,0,m_width - 2*w,m_height};
            m_viewRects.push_back(r1);
            m_viewRects.push_back(r2);
            m_viewRects.push_back(r3);
        }
        else // 4
        {
            int w = m_width / 2;
            int h = m_height / 2;
            SDL_Rect r1{0,0,w,h};
            SDL_Rect r2{w,0,m_width - w,h};
            SDL_Rect r3{0,h,w,m_height - h};
            SDL_Rect r4{w,h,m_width - w,m_height - h};
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

private:
    int m_width = 800;
    int m_height = 600;
    std::vector<SDL_Rect> m_viewRects;
    std::vector<short> m_players; // ordered list of player ids
    std::unordered_map<short,int> m_playerIndexMap;
};
