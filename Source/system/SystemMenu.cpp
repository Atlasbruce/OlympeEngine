#include "SystemMenu.h"
#include <algorithm>
#include <iostream>

SystemMenu::SystemMenu()
{
    // default entries
    m_items.push_back("About");
    m_items.push_back("Settings");
    m_items.push_back("Exit");
}

SystemMenu::~SystemMenu()
{
}

SystemMenu& SystemMenu::GetInstance()
{
    static SystemMenu instance;
    return instance;
}

void SystemMenu::Activate()
{
    if (m_active) return;
    m_active = true;
    // For now we just log to stdout. Integration with UI/SDL will be done later.
    std::cout << "SystemMenu: activated\n";
}

void SystemMenu::Deactivate()
{
    if (!m_active) return;
    m_active = false;
    std::cout << "SystemMenu: deactivated\n";
}

void SystemMenu::Toggle()
{
    if (m_active) Deactivate(); else Activate();
}

void SystemMenu::AddItem(const std::string& item)
{
    if (std::find(m_items.begin(), m_items.end(), item) == m_items.end())
        m_items.push_back(item);
}

void SystemMenu::RemoveItem(const std::string& item)
{
    auto it = std::remove(m_items.begin(), m_items.end(), item);
    if (it != m_items.end()) m_items.erase(it, m_items.end());
}
