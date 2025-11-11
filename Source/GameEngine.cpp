#include "GameEngine.h"
#include "system/EventManager.h"
#include "InputsManager.h"
#include "system/SystemMenu.h"
#include "VideoGame.h"
#include "OptionsManager.h"
#include "DataManager.h"

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
}