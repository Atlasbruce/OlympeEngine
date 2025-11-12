#include "drawing.h"
#include <SDL3/SDL.h>
#include <cmath>

// Portable pi definition for C++14 (avoids M_PI reliance)
static const float PI = static_cast<float>(std::acos(-1.0));

// Color conversion helpers
static inline SDL_FColor ToFColor(const SDL_Color& c)
{
    return SDL_FColor{
        c.r / 255.0f,
        c.g / 255.0f,
        c.b / 255.0f,
        c.a / 255.0f
    };
}

static inline SDL_Color ToColor(const SDL_FColor& f)
{
    auto clamp01 = [](float v) {
        if (v < 0.0f) return 0.0f;
        if (v > 1.0f) return 1.0f;
        return v;
        };
    return SDL_Color{
        static_cast<Uint8>(std::round(clamp01(f.r) * 255.0f)),
        static_cast<Uint8>(std::round(clamp01(f.g) * 255.0f)),
        static_cast<Uint8>(std::round(clamp01(f.b) * 255.0f)),
        static_cast<Uint8>(std::round(clamp01(f.a) * 255.0f))
    };
}
//----------------------------------------------------------
// Draws a circle using the Midpoint Circle Algorithm
void Draw_Circle(SDL_Renderer* renderer, int cx, int cy, int radius)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        SDL_RenderPoint(renderer, (float)cx + x, (float)cy + y);
        SDL_RenderPoint(renderer, (float)cx + y, (float)cy + x);
        SDL_RenderPoint(renderer, (float)cx - y, (float)cy + x);
        SDL_RenderPoint(renderer, (float)cx - x, (float)cy + y);
        SDL_RenderPoint(renderer, (float)cx - x, (float)cy - y);
        SDL_RenderPoint(renderer, (float)cx - y, (float)cy - x);
        SDL_RenderPoint(renderer, (float)cx + y, (float)cy - x);
        SDL_RenderPoint(renderer, (float)cx + x, (float)cy - y);

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
//----------------------------------------------------------
// Draws a filled circle using horizontal scanlines
void Draw_FilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius)
{
    for (int dy = -radius; dy <= radius; ++dy)
    {
        int dx = static_cast<int>(sqrt(radius * radius - dy * dy));
        int x1 = cx - dx;
        int x2 = cx + dx;
        SDL_RenderLine(renderer, (float)x1, (float)cy + dy, (float)x2, (float)cy + dy);
    }
}
//----------------------------------------------------------
// Draws the outline of a triangle
void Draw_Triangle(SDL_Renderer* renderer, SDL_FPoint p1, SDL_FPoint p2, SDL_FPoint p3)
{
    SDL_RenderLine(renderer, p1.x, p1.y, p2.x, p2.y);
    SDL_RenderLine(renderer, p2.x, p2.y, p3.x, p3.y);
    SDL_RenderLine(renderer, p3.x, p3.y, p1.x, p1.y);
}
//----------------------------------------------------------
// Draws a filled triangle using SDL_RenderGeometry
void Draw_FilledTriangle(SDL_Renderer* renderer,
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
//----------------------------------------------------------
// Draws a filled hexagon using SDL_RenderGeometry
void Draw_FilledHexagon(SDL_Renderer* renderer,
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
        float angle = (float)i / numSides * 2.0f * PI;
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
//----------------------------------------------------------
// Draw hexagon outline
void Draw_Hexagon(SDL_Renderer* renderer, SDL_FPoint center, float radius, SDL_Color color)
{
    const int numSides = 6;
    SDL_FPoint verts[numSides];
    for (int i = 0; i < numSides; ++i) {
        float angle = (float)i / numSides * 2.0f * PI;
        verts[i].x = center.x + radius * std::cosf(angle);
        verts[i].y = center.y + radius * std::sinf(angle);
    }

    // Apply color
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Draw lines between consecutive vertices
    for (int i = 0; i < numSides; ++i) {
        int next = (i + 1) % numSides;
        SDL_RenderLine(renderer, verts[i].x, verts[i].y, verts[next].x, verts[next].y);
    }
}
