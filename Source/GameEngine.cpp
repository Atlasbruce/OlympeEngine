#include "GameEngine.h"

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
}