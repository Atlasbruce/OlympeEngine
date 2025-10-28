/* Task.h
 Basic Task for objectives
*/
#pragma once

#include <string>
#include <iostream>
#include "system/system_utils.h"

class Task
{
public:
 std::string description;
 bool completed = false;
 Task(const std::string& d = "Task") : description(d) { SYSTEM_LOG << "Task: " << description << " created\n"; }
 ~Task() { SYSTEM_LOG << "Task: " << description << " destroyed\n"; }

 void Complete() { completed = true; }
};
