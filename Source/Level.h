/* Level.h
 Contains multiple sectors and manages them.
*/
#pragma once

#include "Sector.h"
#include <vector>
#include <memory>
#include <string>
#include <iostream>

class Level
{
public:
 std::string name;
 std::vector<std::unique_ptr<Sector>> sectors;

 Level(const std::string& n = "Level") : name(n)
 {
 std::cout << "Level '" << name << "' created\n";
 }
 ~Level()
 {
 std::cout << "Level '" << name << "' destroyed\n";
 }

 void AddSector(std::unique_ptr<Sector> s)
 {
 sectors.push_back(std::move(s));
 }
};
