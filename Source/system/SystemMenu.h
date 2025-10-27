#pragma once

#include "../Singleton.h"
#include <vector>
#include <string>

// Lightweight System menu singleton used by GameEngine to provide global
// application-level menu controls (show/hide, basic commands).
// This implementation is intentionally minimal and independent from the
// event system so it can be activated simply by calling SystemMenu::Get().

class SystemMenu : public Singleton
{
public:
    SystemMenu();
    virtual ~SystemMenu();

    static SystemMenu& GetInstance();
    static SystemMenu& Get() { return GetInstance(); }

    void Activate();    // make menu active / visible
    void Deactivate();  // hide menu
    void Toggle();
    bool IsActive() const { return m_active; }

    // Basic operations that a system menu might expose
    void AddItem(const std::string& item);
    void RemoveItem(const std::string& item);
    const std::vector<std::string>& Items() const { return m_items; }

private:
    bool m_active = false;
    std::vector<std::string> m_items;
};
