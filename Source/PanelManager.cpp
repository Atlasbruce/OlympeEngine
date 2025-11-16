/*
Olympe Engine V2 2025
Nicolas Chereau
nchereau@gmail.com

Purpose:
- PanelManager implementation. Creates simple Win32 child/floating windows
  to host debug panels (log window, object inspector, tree view). The
  actual rendering/content of windows will be implemented later.

Notes:
- This file uses minimal Win32 APIs guarded by #ifdef _WIN32 so it remains
  portable on other platforms (stubs will be used).
*/

#include "PanelManager.h"
#include "GameEngine.h"
#include "system/log_sink.h"

#ifdef _WIN32
#include <strsafe.h>
// Note: SDL_syswm.h may not be available in all SDKs/environments; don't include it here
#endif

PanelManager::PanelManager()
{
    name = "PanelManager";
    SYSTEM_LOG << "PanelManager created\n";
}

PanelManager::~PanelManager()
{
    SYSTEM_LOG << "PanelManager destroyed\n";
    Shutdown();
}

PanelManager& PanelManager::GetInstance()
{
    static PanelManager instance;
    return instance;
}

void PanelManager::Initialize()
{
#ifdef _WIN32
    // Register a small window class for panels
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = PanelWndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = TEXT("OlympePanelClass");
    m_wndClassAtom = RegisterClassEx(&wc);
    if (!m_wndClassAtom) SYSTEM_LOG << "PanelManager: RegisterClassEx failed\n";
#endif
}

void PanelManager::Shutdown()
{
   // std::lock_guard<std::mutex> lock(m_mutex_);
#ifdef _WIN32
    for (auto &kv : m_panels_)
    {
        if (kv.second.hwnd) DestroyWindow(kv.second.hwnd);
        kv.second.hwnd = nullptr;
        kv.second.hwndChild = nullptr;
    }
    if (m_mainMenu && m_mainHwnd) {
        SetMenu(m_mainHwnd, nullptr);
        DestroyMenu(m_mainMenu);
        m_mainMenu = nullptr;
    }
    if (m_wndClassAtom) UnregisterClass(TEXT("OlympePanelClass"), GetModuleHandle(nullptr));
    m_wndClassAtom = 0;
#else
    m_panels_.clear();
#endif
}

void PanelManager::CreatePanel(const std::string& id, const std::string& title)
{
   // std::lock_guard<std::mutex> lock(m_mutex_);
    auto it = m_panels_.find(id);
    if (it != m_panels_.end()) return; // already created

    Panel p;
    p.id = id;
    p.title = title;
    p.visible = true; // Panels visible by default

#ifdef _WIN32
    // create an overlapped window for tools (initially visible)
    HWND parent = nullptr; // top-level floating
    HWND hwnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_WINDOWEDGE, TEXT("OlympePanelClass"), TEXT(""),
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        parent, nullptr, GetModuleHandle(nullptr), nullptr);
    if (hwnd)
    {
        // set window title
        SetWindowTextA(hwnd, title.c_str());
        p.hwnd = hwnd;

        // if this is the log window, create a child multiline edit control
        if (id == "log_window")
        {
            HWND hEdit = CreateWindowEx(0, TEXT("EDIT"), TEXT(""),
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
                0, 0, 400, 300, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
            if (hEdit)
            {
                // set a readable font
                HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
                SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE,0));
                p.hwndChild = hEdit;
            }
        }

        // show the panel window by default
        ShowWindow(hwnd, SW_SHOW);
        if (p.hwndChild) ShowWindow(p.hwndChild, SW_SHOW);
    }
#endif

    m_panels_.emplace(id, std::move(p));
}

void PanelManager::CreateLogWindow()
{
    CreatePanel("log_window", "Log Window");
}

void PanelManager::CreateObjectInspectorWindow()
{
    CreatePanel("object_inspector", "Object Inspector");
}

void PanelManager::CreateTreeViewWindow()
{
    CreatePanel("tree_view", "Tree View");
}

void PanelManager::ShowPanel(const std::string& id)
{
   // std::lock_guard<std::mutex> lock(m_mutex_);
    auto it = m_panels_.find(id);
    if (it == m_panels_.end()) return;
    it->second.visible = true;
#ifdef _WIN32
    if (it->second.hwnd) ShowWindow(it->second.hwnd, SW_SHOW);
    if (it->second.hwndChild) ShowWindow(it->second.hwndChild, SW_SHOW);
#endif
}

void PanelManager::HidePanel(const std::string& id)
{
   // std::lock_guard<std::mutex> lock(m_mutex_);
    auto it = m_panels_.find(id);
    if (it == m_panels_.end()) return;
    it->second.visible = false;
#ifdef _WIN32
    if (it->second.hwnd) ShowWindow(it->second.hwnd, SW_HIDE);
    if (it->second.hwndChild) ShowWindow(it->second.hwndChild, SW_HIDE);
#endif
}

bool PanelManager::IsPanelVisible(const std::string& id) const
{
   // std::lock_guard<std::mutex> lock(m_mutex_);
    auto it = m_panels_.find(id);
    if (it == m_panels_.end()) return false;
    return it->second.visible;
}

void PanelManager::AppendLog(const std::string& text)
{
    //std::lock_guard<std::mutex> lock(m_mutex_);
    auto it = m_panels_.find("log_window");
    if (it == m_panels_.end()) return;
#ifdef _WIN32
    HWND hEdit = it->second.hwndChild;
    if (!hEdit) return;
    // append text in a thread-safe way using EM_REPLACESEL
    // ensure we append at the end
    int len = GetWindowTextLength(hEdit);
    SendMessage(hEdit, EM_SETSEL, (WPARAM)len, (LPARAM)len);
    // Replace selection with new text
    std::string s = text; // ensure null-terminated
    SendMessageA(hEdit, EM_REPLACESEL, FALSE, (LPARAM)s.c_str());
    // scroll caret into view
    SendMessage(hEdit, EM_SCROLLCARET, 0, 0);
#endif
}

void PanelManager::Process()
{
#ifdef _WIN32
    // simple message pump for panel windows (non-blocking)
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        // Intercept menu commands from main window if attached
        if (msg.message == WM_COMMAND && m_mainHwnd != nullptr && msg.hwnd == m_mainHwnd)
        {
            int id = LOWORD(msg.wParam);
            switch (id)
            {
            case IDM_PANEL_LOG:
                if (IsPanelVisible("log_window")) HidePanel("log_window"); else ShowPanel("log_window");
                continue; // do not dispatch
            case IDM_PANEL_INSPECTOR:
                if (IsPanelVisible("object_inspector")) HidePanel("object_inspector"); else ShowPanel("object_inspector");
                continue;
            case IDM_PANEL_TREE:
                if (IsPanelVisible("tree_view")) HidePanel("tree_view"); else ShowPanel("tree_view");
                continue;
            default:
                break;
            }
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#endif
}

#ifdef _WIN32
void PanelManager::AttachToSDLWindow(SDL_Window* /*sdlWindow*/)
{
    // AttachToSDLWindow is a no-op in this build (SDL syswm not available)
    // If SDL_syswm.h is available, this method can be extended to obtain
    // the native HWND and attach a menu to the SDL window.
}

LRESULT CALLBACK PanelManager::PanelWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        ShowWindow(hwnd, SW_HIDE);
        return 0;
    case WM_SIZE:
    {
        // resize child content if present
        PanelManager& mgr = PanelManager::GetInstance();
       // std::lock_guard<std::mutex> lock(mgr.m_mutex_);
        for (auto &kv : mgr.m_panels_)
        {
            if (kv.second.hwnd == hwnd)
            {
                HWND hChild = kv.second.hwndChild;
                if (hChild)
                {
                    RECT rc; GetClientRect(hwnd, &rc);
                    MoveWindow(hChild, 0, 0, rc.right - rc.left, rc.bottom - rc.top, TRUE);
                }
                break;
            }
        }
        return 0;
    }
    case WM_DESTROY:
        return 0;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

// bridge for the log sink to call into PanelManager implementation
namespace SystemLogSink
{
    void AppendToLogWindow(const std::string& text)
    {
        PanelManager::GetInstance().AppendLog(text);
    }
}

#endif
