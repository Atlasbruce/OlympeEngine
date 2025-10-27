#pragma once

#include "../Object.h"
#include "../system/system_consts.h"
#include <vector>
#include <string>

// GameMenu is an instance-based menu attached to a VideoGame object.
// It receives events (via Object::OnEvent forwarding) and can be used
// to navigate a set of menu entries. It is also a GameObject-like
// entity so it can be added to the engine's object list if desired.

class GameMenu : public Object
{
public:
    GameMenu();
    virtual ~GameMenu();

    void Activate();
    void Deactivate();
    bool IsActive() const { return m_active; }

    void AddEntry(const std::string& e) { m_entries.push_back(e); }

    // Called by EventManager when messages arrive (overrides Object::OnEvent)
    virtual void OnEvent(const Message& msg) override;

    // Render simplified text-based menu (placeholder)
    virtual void Render() override;

private:
    bool m_active = false;
    std::vector<std::string> m_entries;
    int m_selected = 0;
};
