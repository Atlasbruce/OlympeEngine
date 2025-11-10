/*
purpose: This file contains functions related to drawing shapes and images on the screen.
*/

#include <SDL3/SDL.h>
#include "system/system_consts.h"
#include <cmath>


static void Draw_Circle(SDL_Renderer* renderer, int cx, int cy, int radius)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        SDL_RenderPoint(renderer, static_cast<float>(cx + x), static_cast<float>(cy + y));
        SDL_RenderPoint(renderer, static_cast<float>(cx + y), static_cast<float>(cy + x));
        SDL_RenderPoint(renderer, static_cast<float>(cx - y), static_cast<float>(cy + x));
        SDL_RenderPoint(renderer, static_cast<float>(cx - x), static_cast<float>(cy + y));
        SDL_RenderPoint(renderer, static_cast<float>(cx - x), static_cast<float>(cy - y));
        SDL_RenderPoint(renderer, static_cast<float>(cx - y), static_cast<float>(cy - x));
        SDL_RenderPoint(renderer, static_cast<float>(cx + y), static_cast<float>(cy - x));
        SDL_RenderPoint(renderer, static_cast<float>(cx + x), static_cast<float>(cy - y));

        if (err <= 0)
        {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}


// Draws a filled circle using horizontal scanlines
static void Draw_FilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius)
{
    for (int dy = -radius; dy <= radius; ++dy)
    {
        int dx = static_cast<int>(sqrt(radius * radius - dy * dy));
        int x1 = cx - dx;
        int x2 = cx + dx;
        SDL_RenderLine(renderer, static_cast<float> (x1), static_cast<float>(cy + dy), static_cast<float>(x2), static_cast<float>(cy + dy));
    }
}

static void Draw_Triangle(SDL_Renderer* renderer, SDL_FPoint p1, SDL_FPoint p2, SDL_FPoint p3)
{
    SDL_RenderLine(renderer, p1.x, p1.y, p2.x, p2.y);
    SDL_RenderLine(renderer, p2.x, p2.y, p3.x, p3.y);
    SDL_RenderLine(renderer, p3.x, p3.y, p1.x, p1.y);
}

static void Draw_FilledTriangle(SDL_Renderer* renderer,
    const SDL_FPoint& p1,
    const SDL_FPoint& p2,
    const SDL_FPoint& p3,
    SDL_FColor color)
{
    SDL_Vertex vertices[3];

    vertices[0].position = p1;
    vertices[1].position = p2;
    vertices[2].position = p3;

    vertices[0].color = color;
    vertices[1].color = color;
    vertices[2].color = color;

    vertices[0].tex_coord = { 0, 0 };
    vertices[1].tex_coord = { 0, 0 };
    vertices[2].tex_coord = { 0, 0 };

    SDL_RenderGeometry(renderer, nullptr, vertices, 3, nullptr, 0);
}

static void Draw_Hexagon(SDL_Renderer* renderer,
    SDL_FPoint center,
    float radius,
    SDL_Color color)
{
    const int numSides = 6;
    SDL_FPoint points[numSides + 1]; // +1 pour boucler sur le premier point

    // Calcul des sommets de l’hexagone
    for (int i = 0; i < numSides; ++i) {
        float angle = (float)i / numSides * 2.0f * k_PI;
        points[i].x = center.x + radius * std::cos(angle);
        points[i].y = center.y + radius * std::sin(angle);
    }

    // Boucler pour fermer la forme (dernier segment jusqu’au premier)
    points[numSides] = points[0];

    // Dessine les segments
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int i = 0; i < numSides; ++i) {
        SDL_RenderLine(renderer,
            points[i].x, points[i].y,
            points[i + 1].x, points[i + 1].y);
    }
}

static void Draw_FilledHexagon(SDL_Renderer* renderer,
    SDL_FPoint center,
    float radius,
    SDL_FColor color)
{
    const int numSides = 6;
    SDL_Vertex vertices[numSides + 1];
    int indices[numSides * 3];

    // Le centre du polygone
    vertices[0].position = center;
    vertices[0].color = color;
    vertices[0].tex_coord = { 0, 0 };

    // Calcul des sommets sur le cercle
    for (int i = 0; i < numSides; ++i) {
        float angle = static_cast<float>(i / numSides * 2.0f * k_PI);
        vertices[i + 1].position = {
            center.x + radius * std::cosf(angle),
            center.y + radius * std::sinf(angle)
        };
        vertices[i + 1].color = color;
        vertices[i + 1].tex_coord = { 0, 0 };
    }

    // Crée 6 triangles reliant le centre et les sommets du bord
    for (int i = 0; i < numSides; ++i) {
        int next = (i + 1) % numSides;
        indices[i * 3 + 0] = 0;           // centre
        indices[i * 3 + 1] = i + 1;       // sommet actuel
        indices[i * 3 + 2] = next + 1;    // sommet suivant
    }

    // Rendu de la géométrie
    SDL_RenderGeometry(renderer, nullptr, vertices, numSides + 1, indices, numSides * 3);
}