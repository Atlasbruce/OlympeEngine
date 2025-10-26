/*
Olympe Engine V2 2025
Nicolas Chereau
nchereau@gmail.com

Purpose:
- Singleton design pattern implementation.
- Ensures a class has only one instance and provides a global point of access to it.

*/
#pragma once
class Singleton
{
	static Singleton* instance;
public:	
	static Singleton* getInstance()
	{
		if (instance == nullptr)
		{
			instance = new Singleton();
		}
		return instance;
	}
};

