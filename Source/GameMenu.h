/* GameMenu.h
 Simple stub for a game menu
*/
#pragma once

#include <string>
#include <iostream>

class GameMenu
{
public:
 GameMenu() { std::cout << "GameMenu created\n"; }
 ~GameMenu() { std::cout << "GameMenu destroyed\n"; }

 void Show() { std::cout << "Showing GameMenu\n"; }
};
