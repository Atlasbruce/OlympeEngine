/*
purpose: This file contains functions related to drawing shapes and images on the screen.

Performance Notes:
- Draw_FilledCircle: Optimized to use integer arithmetic and minimize sqrt calls
- Draw_Hexagon/Draw_FilledHexagon: Use pre-calculated trigonometric values for better performance
*/
#pragma once
#include <SDL3/SDL.h>


static inline SDL_Color ToColor(const SDL_FColor&);
static inline SDL_FColor ToFColor(const SDL_Color&);

// Draws circle outline using Midpoint Circle Algorithm
void Draw_Circle(SDL_Renderer* renderer, int cx, int cy, int radius);

// Draws filled circle using horizontal scanlines (optimized)
void Draw_FilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius);

void Draw_Triangle(SDL_Renderer* renderer, SDL_FPoint p1, SDL_FPoint p2, SDL_FPoint p3);
void Draw_FilledTriangle(SDL_Renderer* renderer,
    const SDL_FPoint& p1,
    const SDL_FPoint& p2,
    const SDL_FPoint& p3,
	SDL_FColor color);

// Draws hexagon outline (optimized with pre-calculated trig values)
void Draw_Hexagon(SDL_Renderer* renderer,
    SDL_FPoint center,
    float radius,
	SDL_Color color);

// Draws filled hexagon (optimized with pre-calculated trig values)
void Draw_FilledHexagon(SDL_Renderer* renderer,
    SDL_FPoint center,
	float radius,
	SDL_FColor color);
