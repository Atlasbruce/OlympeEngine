/* GameRules.h
 Defines rules for a videogame (placeholder).
*/
#pragma once

#include <string>
#include <iostream>

class GameRules
{
public:
 std::string name;
 GameRules(const std::string& n = "DefaultRules") : name(n) { std::cout << "GameRules '" << name << "' created\n"; }
 ~GameRules() { std::cout << "GameRules '" << name << "' destroyed\n"; }

 // Placeholder for rule checks
 bool Validate() const { return true; }
};
