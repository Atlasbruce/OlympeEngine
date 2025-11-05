/*
Olympe Engine V2 2025
Nicolas Chereau
nchereau@gmail.com

Purpose:
- Singleton design pattern implementation.
- Ensures a class has only one instance and provides a global point of access to it.
- Base Singleton marker class (no instance managed here).
- Individual derived classes should implement their own GetInstance()/Get() methods.

*/


/*
* DEPRECATED : Use Object with ObjectType::Singleton instead.

#pragma once
#include <string>

struct Message;

class Singleton
{
public:
	Singleton() = default;
	virtual ~Singleton() = default;

	virtual void Process() {};
	virtual void Render() {};
	virtual void OnEvent(const Message&) {};

	std::string name = "unnamed singleton";
};

*/