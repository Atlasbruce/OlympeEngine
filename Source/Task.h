/* Task.h
 Basic Task for objectives
*/
#pragma once

#include <string>
#include <iostream>

class Task
{
public:
 std::string description;
 bool completed = false;
 Task(const std::string& d = "Task") : description(d) { std::cout << "Task: " << description << " created\n"; }
 ~Task() { std::cout << "Task: " << description << " destroyed\n"; }

 void Complete() { completed = true; }
};
