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

	~Singleton() = default;

public:	
	static Singleton* get()
	{
		if (instance == nullptr)
		{
			instance = new Singleton();
		}
		return instance;
	}

	virtual void Process() {};
	virtual void Render() {};
	virtual void OnEvent() {};

	// Properties
	static Singleton* instance;
};

Singleton* Singleton::instance = nullptr;