/* Objective.h
 Objective contains tasks
*/
#pragma once

#include "Task.h"
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include "system/system_utils.h"

class Objective
{
public:
 std::string title;
 std::vector<std::unique_ptr<Task>> tasks;
 Objective(const std::string& t = "Objective") : title(t) { SYSTEM_LOG << "Objective: " << title << " created\n"; }
 ~Objective() { SYSTEM_LOG << "Objective: " << title << " destroyed\n"; }

 void AddTask(std::unique_ptr<Task> task) { tasks.push_back(std::move(task)); }
 bool IsComplete() const
 {
 for (const auto& t : tasks) if (!t->completed) return false;
 return true;
 }
};
