/*
Olympe Engine V2 2025
Nicolas Chereau
nchereau@gmail.com

Purpose:
- PanelManager is a singleton responsible for creating and managing
  lightweight tool windows (log window, object inspector, tree view)
  using the Win32 API when available. The actual rendering/content of
  these windows is implemented later; this class provides creation,
  visibility and lifetime management.

Notes:
- On non-Windows platforms the implementation is a no-op (stubs), so
  the engine remains portable.
*/

#pragma once

#include "object.h"
#include "system/system_utils.h"
#include <string>
#include <unordered_map>
#include <mutex>

#ifdef _WIN32
#include <windows.h>
#endif

class PanelManager : public Object
{
public:
    PanelManager();
    virtual ~PanelManager();

    virtual ObjectType GetObjectType() const override { return ObjectType::Singleton; }

    static PanelManager& GetInstance();
    static PanelManager& Get() { return GetInstance(); }

    // Initialize/shutdown the manager (register window class on Windows)
    void Initialize();
    void Shutdown();

    // Create predefined panels (id is used to reference)
    void CreateLogWindow();
    void CreateObjectInspectorWindow();
    void CreateTreeViewWindow();

    // Show/hide panels
    void ShowPanel(const std::string& id);
    void HidePanel(const std::string& id);
    bool IsPanelVisible(const std::string& id) const;

    // Process internal tasks (on Windows: pump messages for tool windows)
    void Process();

private:
    struct Panel
    {
        std::string id;
        std::string title;
        bool visible = false;
#ifdef _WIN32
        HWND hwnd = nullptr;
#else
        void* hwnd = nullptr; // stub
#endif
    };

    void CreatePanel(const std::string& id, const std::string& title);

    mutable std::mutex m_mutex_;
    std::unordered_map<std::string, Panel> m_panels_;

#ifdef _WIN32
    ATOM m_wndClassAtom = 0;
    static LRESULT CALLBACK PanelWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
};
