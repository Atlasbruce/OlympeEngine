#include "GameMenu.h"
#include "EventManager.h"
#include "../VideoGame.h"
#include <iostream>

GameMenu::GameMenu()
{
}

GameMenu::~GameMenu()
{
}

void GameMenu::Activate()
{
    if (m_active) return;
    m_active = true;
    // register to listen for menu events
    EventManager::Get().Register(this, EventType::EventType_Menu_Validate);
    EventManager::Get().Register(this, EventType::EventType_Menu_Enter);
    EventManager::Get().Register(this, EventType::EventType_Menu_Exit);

    // Pause the game when the menu is activated
    VideoGame::Get().Pause();

    std::cout << "GameMenu: activated\n";
}

void GameMenu::Deactivate()
{
    if (!m_active) return;
    m_active = false;
    EventManager::Get().Unregister(this, EventType::EventType_Menu_Validate);
    EventManager::Get().Unregister(this, EventType::EventType_Menu_Enter);
    EventManager::Get().Unregister(this, EventType::EventType_Menu_Exit);

    // Resume the game when the menu is closed
    VideoGame::Get().Resume();

    std::cout << "GameMenu: deactivated\n";
}

void GameMenu::OnEvent(const Message& msg)
{
    // simple handler reacting to menu events
    switch (msg.type)
    {
        case EventType::EventType_Menu_Enter:
            Activate();
            std::cout << "GameMenu: Enter event - activated\n";
            break;
        case EventType::EventType_Menu_Exit:
            Deactivate();
            std::cout << "GameMenu: Exit event - deactivated\n";
            break;
        case EventType::EventType_Menu_Validate:
            if (m_active)
            {
                std::cout << "GameMenu: Validate item '" << (m_entries.size()>0?m_entries[m_selected]:std::string("<none>")) << "'\n";
                // If the validated item is Exit, request the game to quit
                if (m_entries.size() > 0 && m_entries[m_selected] == "Exit")
                {
                    VideoGame::Get().RequestQuit();
                }
            }
            break;
        default:
            break;
    }
}

void GameMenu::Render()
{
    if (!m_active) return;
    std::cout << "--- GameMenu ---\n";
    for (size_t i = 0; i < m_entries.size(); ++i)
    {
        if (static_cast<int>(i) == m_selected) std::cout << "> "; else std::cout << "  ";
        std::cout << m_entries[i] << "\n";
    }
}
