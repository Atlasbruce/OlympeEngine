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

#ifdef _WIN32
#include <strsafe.h>
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
    std::lock_guard<std::mutex> lock(m_mutex_);
#ifdef _WIN32
    for (auto &kv : m_panels_)
    {
        if (kv.second.hwnd) DestroyWindow(kv.second.hwnd);
        kv.second.hwnd = nullptr;
    }
    if (m_wndClassAtom) UnregisterClass(TEXT("OlympePanelClass"), GetModuleHandle(nullptr));
    m_wndClassAtom = 0;
#else
    m_panels_.clear();
#endif
}

void PanelManager::CreatePanel(const std::string& id, const std::string& title)
{
    std::lock_guard<std::mutex> lock(m_mutex_);
    auto it = m_panels_.find(id);
    if (it != m_panels_.end()) return; // already created

    Panel p;
    p.id = id;
    p.title = title;
    p.visible = true;

#ifdef _WIN32
    // create an overlapped window for tools (initially hidden)
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
    std::lock_guard<std::mutex> lock(m_mutex_);
    auto it = m_panels_.find(id);
    if (it == m_panels_.end()) return;
    it->second.visible = true;
#ifdef _WIN32
    if (it->second.hwnd) ShowWindow(it->second.hwnd, SW_SHOW);
#endif
}

void PanelManager::HidePanel(const std::string& id)
{
    std::lock_guard<std::mutex> lock(m_mutex_);
    auto it = m_panels_.find(id);
    if (it == m_panels_.end()) return;
    it->second.visible = false;
#ifdef _WIN32
    if (it->second.hwnd) ShowWindow(it->second.hwnd, SW_HIDE);
#endif
}

bool PanelManager::IsPanelVisible(const std::string& id) const
{
    std::lock_guard<std::mutex> lock(m_mutex_);
    auto it = m_panels_.find(id);
    if (it == m_panels_.end()) return false;
    return it->second.visible;
}

void PanelManager::Process()
{
#ifdef _WIN32
    // simple message pump for panel windows (non-blocking)
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#endif
}

#ifdef _WIN32
LRESULT CALLBACK PanelManager::PanelWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        ShowWindow(hwnd, SW_HIDE);
        return 0;
    case WM_SIZE:
        // TODO: handle resize for child content
        return 0;
    case WM_DESTROY:
        return 0;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
#endif
