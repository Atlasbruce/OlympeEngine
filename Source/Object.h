/*
Olympe Engine V2 2025
Nicolas Chereau
nchereau@gmail.com

Purpose:
- Class representing a generic object in a game engine, with enumerations for object types and states.

*/
#pragma once
#include <string>
#include "system/message.h"
#include <chrono>
#include <cstdint>

enum class ObjectType
{
	None = 0,
	Menu,
	Player,
	Enemy,
	NPC,
	Item,
	Environment,
	Projectile, 
	Sector,
	Level,
	Trigger,
	Collectible,
	
};

enum class ObjectState
{
	Idle = 0,
	Moving,
	Jumping,
	Attacking,
	Dead,
	Interacting, 
	Collecting,
	Probing,
	Exploring,
	Hiding,
};

class Object
{
public:
	Object()
	{
		// generate a unique id based on current time in nanoseconds
		using namespace std::chrono;
		auto now = system_clock::now();
		uid = static_cast<uint64_t>(duration_cast<nanoseconds>(now.time_since_epoch()).count());
	}
	virtual ~Object() {};

public:

	// Object Type
	ObjectType type = ObjectType::None;
	// Object State
	ObjectState state = ObjectState::Idle;
	std::string name = "unnamed_object";

	virtual void Process() {};
	virtual void Render() {};
	virtual void OnEvent(const Message& msg) {};

	// comparaison operator
	inline virtual bool	operator==	(Object& _oc) { return (this == &_oc); }
	inline virtual bool	operator!=	(Object& _oc) { return (this != &_oc); }
	inline virtual bool	operator>	(Object& _oc) { return false; }
	inline virtual bool	operator<	(Object& _oc) { return false; }
	inline virtual bool	operator>=	(Object& _oc) { return false; }
	inline virtual bool	operator<=	(Object& _oc) { return false; }

	inline uint64_t GetUID() const { return uid; }

protected:
	// Object UID
	uint64_t uid = 0;
};


