/*
	engine_utils.h
	Nicolas Chereau 2006
	Olympe Engine ::2D Game Editor ::olympe.editor@gmail.com
	nchereau@gmail.com
*/

#pragma once

#include "vector.h"
#include <SDL3/SDL.h>
#include <random>

//////////////////////////////////////////////////////////
// MACROS 
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

//////////////////////////////////////////////////////////
//MESSAGE & LOG
void MsgBox(string& stitle, string& smsg, bool _berror = false);

//////////////////////////////////////////////////////////
// RANDOM
inline int Random_Int(const int a, const int b) { return SDL_rand(b - a); }
inline float Random_Sign(void) {if (Random_Int(0, 1)) return 1.0f; else return -1.0f;}
inline float Random_Float(const float a, const float b) {return (float) (Random_Int((int)a, (int)b) + SDL_randf() );}

inline Vector Random_Vector(const float a, const float b) {return Vector(Random_Float(a, b),Random_Float(a, b),Random_Float(a, b));}
inline Vector Random_Point(const Vector p1, const Vector p2) {return Vector(Random_Float(p1.x, p2.x),Random_Float(p1.y, p2.y),Random_Float(p1.z, p2.z));}
inline Vector Random_Point(const Vector p, const float r) {return Vector(p.x + Random_Float(0.0f, r) * Random_Sign(), p.y + Random_Float(0.0f, r) * Random_Sign(), p.z + Random_Float(0.0f, r) * Random_Sign());}
inline SDL_Color Random_Color(const int a = 0, const int b = 255) { SDL_Color col /*RGBA*/ = { (unsigned char)Random_Int(a, b), (unsigned char)Random_Int(a, b), (unsigned char)Random_Int(a, b), 0xFF }; return col; }

//////////////////////////////////////////////////////////
// Clamp a float into a range
float FloatClamp (float _f, float fMin, float fMax);
int IntClamp (int _i, int iMin, int iMax);
Vector VectorClamp(Vector _v, Vector _vMin, Vector _vMax);
float fBlend ( float _f1, float _f2, float _fc);


