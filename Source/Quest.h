/* Quest.h
 Quest contains objectives
*/
#pragma once

#include "Objective.h"
#include <vector>
#include <memory>
#include <string>
#include <iostream>

class Quest
{
public:
 std::string name;
 std::vector<std::unique_ptr<Objective>> objectives;
 Quest(const std::string& n = "Quest") : name(n) { std::cout << "Quest '" << name << "' created\n"; }
 ~Quest() { std::cout << "Quest '" << name << "' destroyed\n"; }

 void AddObjective(std::unique_ptr<Objective> o) { objectives.push_back(std::move(o)); }
 bool IsComplete() const
 {
 for (const auto& o : objectives) if (!o->IsComplete()) return false;
 return true;
 }
};
