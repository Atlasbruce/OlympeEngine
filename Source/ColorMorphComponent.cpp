#include "ColorMorphComponent.h"
#include "Factory.h"
#include "GameEngine.h"
#include <cmath>
#include <cstdlib>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>

bool ColorMorphComponent::FactoryRegistered = Factory::Get().Register("ColorMorphComponent", ColorMorphComponent::Create);
ObjectComponent* ColorMorphComponent::Create()
{
	return new ColorMorphComponent();
}

//---------------------------------------------------------
// Helper function for color interpolation
static Uint8 LerpColor(Uint8 a, Uint8 b, float t)
{
    return static_cast<Uint8>(a + (b - a) * t);
}

ColorMorphComponent::ColorMorphComponent()
{
    width = 800;
    height = 600;
    
    morphTexture = SDL_CreateTexture(GameEngine::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
}

ColorMorphComponent::~ColorMorphComponent()
{
    SDL_DestroyTexture(morphTexture);
}

void ColorMorphComponent::Initialize()
{
    // Initialize a few random color points
    points.clear();
    for (int i = 0; i < 4; ++i) {
        ColorPoint cp;
        cp.x = static_cast<float>(rand()) / RAND_MAX;
        cp.y = static_cast<float>(rand()) / RAND_MAX;
        cp.color = { Uint8(rand() % 256), Uint8(rand() % 256), Uint8(rand() % 256), 255 };
        points.push_back(cp);
    }
}

void ColorMorphComponent::OnEvent(const Message& msg)
{
	// Handle events if needed (e.g., reset colors on specific event)
}

void ColorMorphComponent::Process()
{
    // Slowly animate color positions and hues
    time += fDt;
    for (auto& cp : points) {
        cp.x = 0.5f + 0.5f * sin(time + (&cp - &points[0]));
        cp.y = 0.5f + 0.5f * cos(time * 0.7f + (&cp - &points[0]));
        cp.color.r = Uint8(127 + 127 * sin(time + cp.x * 3));
        cp.color.g = Uint8(127 + 127 * sin(time * 1.2f + cp.y * 2));
        cp.color.b = Uint8(127 + 127 * sin(time * 0.8f + cp.x * cp.y * 4));
    }
}

void ColorMorphComponent::Render()
{
    //SDL_SetRenderTarget(GameEngine::renderer, morphTexture);
    GenerateGradient();
    ApplyBlur(morphTexture, 3); // Apply a soft blur (simulates bloom)
    //SDL_SetRenderTarget(GameEngine::renderer, nullptr);

    // Draw final texture to the main renderer
    SDL_RenderTexture(GameEngine::GameEngine::renderer, morphTexture, nullptr, nullptr);
}

void ColorMorphComponent::GenerateGradient()
{
    // Fill texture with interpolated colors based on color points
    SDL_SetRenderDrawBlendMode(GameEngine::renderer, SDL_BLENDMODE_BLEND);
    //SDL_SetRenderDrawColor(GameEngine::renderer, 0, 0, 0, 255);
    //SDL_RenderClear(GameEngine::renderer);

    const int steps = 6;
    for (auto& cp : points) {
        SDL_SetRenderDrawColor(GameEngine::renderer, cp.color.r, cp.color.g, cp.color.b, 180);
        int radius = width / steps;
        SDL_FRect circleRect = { cp.x * width - radius / 2, cp.y * height - radius / 2, float(radius), float(radius) };
        SDL_RenderFillRect(GameEngine::renderer, &circleRect);
    }
}

void ColorMorphComponent::ApplyBlur(SDL_Texture* texture, int passes)
{
    // Simple blur: multiple passes with semi-transparent overlays
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    for (int i = 0; i < passes; ++i) {
        SDL_SetTextureAlphaMod(texture, 180 - i * 40);
        SDL_RenderTexture(GameEngine::renderer, texture, nullptr, nullptr);
    }
}