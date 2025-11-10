/*
purpose: This file contains functions related to drawing shapes and images on the screen.
*/

#include <SDL3/SDL.h>


static inline SDL_Color ToColor(const SDL_FColor&);
static inline SDL_FColor ToFColor(const SDL_Color&);
void Draw_Circle(SDL_Renderer* renderer, int cx, int cy, int radius);
void Draw_FilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius);
void Draw_Triangle(SDL_Renderer* renderer, SDL_FPoint p1, SDL_FPoint p2, SDL_FPoint p3);
void Draw_FilledTriangle(SDL_Renderer* renderer,
    const SDL_FPoint& p1,
    const SDL_FPoint& p2,
    const SDL_FPoint& p3,
	SDL_FColor color);
void Draw_Hexagon(SDL_Renderer* renderer,
    SDL_FPoint center,
    float radius,
	SDL_Color color);
void Draw_FilledHexagon(SDL_Renderer* renderer,
    SDL_FPoint center,
	float radius,
	SDL_FColor color);
