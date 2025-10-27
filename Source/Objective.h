/* Objective.h
 Objective contains tasks
*/
#pragma once

#include "Task.h"
#include <vector>
#include <memory>
#include <string>
#include <iostream>

class Objective
{
public:
 std::string title;
 std::vector<std::unique_ptr<Task>> tasks;
 Objective(const std::string& t = "Objective") : title(t) { std::cout << "Objective: " << title << " created\n"; }
 ~Objective() { std::cout << "Objective: " << title << " destroyed\n"; }

 void AddTask(std::unique_ptr<Task> task) { tasks.push_back(std::move(task)); }
 bool IsComplete() const
 {
 for (const auto& t : tasks) if (!t->completed) return false;
 return true;
 }
};
