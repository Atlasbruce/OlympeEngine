#include "GameEngine.h"
#include "system/EventManager.h"
#include "InputsManager.h"
#include "system/SystemMenu.h"
#include "VideoGame.h"
#include "OptionsManager.h"
#include "DataManager.h"
#include "PanelManager.h"

float GameEngine::fDt = 0.0f;
SDL_Renderer* GameEngine::renderer = nullptr;
int GameEngine::screenWidth = 1280;
int GameEngine::screenHeight = 720;

void GameEngine::Initialize()
{
	ptr_eventmanager = &EventManager::GetInstance();
	ptr_inputsmanager = &InputsManager::GetInstance();
	ptr_systemmenu = &SystemMenu::GetInstance();
	ptr_videogame = &VideoGame::GetInstance();
	ptr_optionsmanager = &OptionsManager::GetInstance();
	ptr_datamanager = &DataManager::GetInstance();

	// Preload system resources from olymp.ini
	DataManager::Get().PreloadSystemResources("./olympe.ini");

	// Create and initialize panel manager
	PanelManager::Get().Initialize();
	PanelManager::Get().CreateLogWindow();
	PanelManager::Get().CreateObjectInspectorWindow();
	PanelManager::Get().CreateTreeViewWindow();
	// By default keep them hidden; can be shown by the UI later
}