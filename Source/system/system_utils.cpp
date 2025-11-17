/*
Olympus Game Engine V2 2025
Nicolas Chereau
nchereau@gmail.com

purpose:

Notes:

*/

#include "system_utils.h"
#include "..\gameengine.h"
#include "..\PanelManager.h"

void LoadOlympeConfig(const char* filename)
{
    GameEngine::screenWidth = DEFAULT_WINDOW_WIDTH;
    GameEngine::screenHeight = DEFAULT_WINDOW_HEIGHT;

    std::ifstream ifs(filename);
    if (!ifs) {
        SYSTEM_LOG << "Config file '" << filename << "' not found — using defaults " << GameEngine::screenWidth << "x" << GameEngine::screenHeight << "\n";
        return;
    }

    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	// Extract screen width and height
    int w = GameEngine::screenWidth;
    int h = GameEngine::screenHeight;

    if (extract_json_int(content, "screen_width", w) || extract_json_int(content, "screenWidth", w) || extract_json_int(content, "width", w)) {}
    if (extract_json_int(content, "screen_height", h) || extract_json_int(content, "screenHeight", h) || extract_json_int(content, "screen_heigth", h) || extract_json_int(content, "height", h)) {}

    if (w > 0) GameEngine::screenWidth = w;
    if (h > 0) GameEngine::screenHeight = h;

    SYSTEM_LOG << "Config loaded from '" << filename << "': " << GameEngine::screenWidth << "x" << GameEngine::screenHeight << "\n";

	// Extract Log Panel data
	if (extract_json_int(content, "log_panel_width", PanelManager::LogPanelWidth)) {}
    if (extract_json_int(content, "log_panel_height", PanelManager::LogPanelHeight)) {}
	if (extract_json_int(content, "log_panel_posx", PanelManager::LogPanelPosX)) {}
	if (extract_json_int(content, "log_panel_posy", PanelManager::LogPanelPosY)) {}

	// Extract Object Inspector Panel data
	if (extract_json_int(content, "inspector_panel_width", PanelManager::InspectorPanelWidth)) {}
    if (extract_json_int(content, "inspector_panel_height", PanelManager::InspectorPanelHeight)) {}
	if (extract_json_int(content, "inspector_panel_posx", PanelManager::InspectorPanelPosX)) {}
	if (extract_json_int(content, "inspector_panel_posy", PanelManager::InspectorPanelPosY)) {}

	// Extract Tree View Panel data
	if (extract_json_int(content, "treeview_panel_width", PanelManager::TreeViewPanelWidth)) {}
    if (extract_json_int(content, "treeview_panel_height", PanelManager::TreeViewPanelHeight)) {}
	if (extract_json_int(content, "treeview_panel_posx", PanelManager::TreeViewPanelPosX)) {}
	if (extract_json_int(content, "treeview_panel_posy", PanelManager::TreeViewPanelPosY)) {}

      /*  "object_panel_width" : 200,
        "object_panel_height" : 400,
        "object_panel_posx" : 700,
        "object_panel_posy" : 0,

        "treeview_panel_width" : 300,
        "treeview_panel_height" : 600,
        "treeview_panel_posx" : 700,
        "treeview_panel_posy" : 400,/**/
}