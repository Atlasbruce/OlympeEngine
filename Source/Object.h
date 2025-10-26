/*
Olympe Engine V2 2025
Nicolas Chereau
nchereau@gmail.com

Purpose:
- Class representing a generic object in a game engine, with enumerations for object types and states.

*/
#pragma once
#include <string>

enum class ObjectType
{
	None = 0,
	Player,
	Enemy,
	NPC,
	Item,
	Environment,
	Projectile, 
	Sector,
	Level,
	World,
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
	Object() = default;
	virtual ~Object() {};

public:
	// Object UID
	int uid = 0;

	// Object Type
	ObjectType type = ObjectType::None;
	// Object State
	ObjectState state = ObjectState::Idle;
	std::string name;

	virtual void Process() {};
	virtual void Render() {};
	virtual void OnEvent() {};

	// comparaison operator
	inline virtual bool	operator==	(Object& _oc) { return (this == &_oc); }
	inline virtual bool	operator!=	(Object& _oc) { return (this != &_oc); }
	inline virtual bool	operator>	(Object& _oc) { return false; }
	inline virtual bool	operator<	(Object& _oc) { return false; }
	inline virtual bool	operator>=	(Object& _oc) { return false; }
	inline virtual bool	operator<=	(Object& _oc) { return false; }
};


