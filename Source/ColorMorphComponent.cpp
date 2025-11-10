#include "ColorMorphComponent.h"
#include "Factory.h"
#include "GameEngine.h"
#include <cmath>
#include <cstdlib>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
//#include <SDL3/SDL_image.h>
#include "drawing.h"

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
    width = GameEngine::screenWidth;
    height = GameEngine::screenHeight;
    
    morphTexture = SDL_CreateTexture(GameEngine::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    SDL_SetTextureBlendMode(morphTexture, SDL_BLENDMODE_BLEND);
    Initialize();
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

    SDL_Surface *Olympe_Logo = SDL_LoadBMP(".\\Resources\\olympe_logo.bmp");
    if (Olympe_Logo) 
    {
        SDL_Texture* logoTexture = SDL_CreateTextureFromSurface(GameEngine::renderer, Olympe_Logo);
        SDL_DestroySurface(Olympe_Logo);

        if (!logoTexture) 
        {
            SYSTEM_LOG << "Failed to create logo texture from surface: " << SDL_GetError() << "\n";
        }
        else 
        {
			this->logoTexture = logoTexture;
        }
	}
    else 
    {
        SYSTEM_LOG << "Failed to load logo image: " << SDL_GetError() << "\n";
	}
}

void ColorMorphComponent::OnEvent(const Message& msg)
{
	// Handle events if needed (e.g., reset colors on specific event)
}

void ColorMorphComponent::Process()
{
    // Slowly animate color positions and hues
    time += fDt * 0.5f;
    for (auto& cp : points) {
        cp.x = 0.5f + 0.5f * sin(time + (&cp - &points[0]));
        cp.y = 0.5f + 0.5f * cos(time * 0.7f + (&cp - &points[0]));
        cp.color.r = Uint8(127 + 127 * sin(time + cp.x * 3));
        cp.color.g = Uint8(127 + 127 * sin(time * 1.2f + cp.y * 2));
        cp.color.b = Uint8(127 + 127 * sin(time * 0.8f + cp.x * cp.y * 4));
    }
/**/

    // Set the texture as render target
    SDL_SetRenderTarget(GameEngine::renderer, morphTexture);

	//we can Draw on the texture now
    SDL_FColor mColor = { 1.f, 1.f, 1.f, 1.f };
    SDL_FPoint mPos = { width * 0.5f, height * 0.5f };

	SDL_SetRenderDrawColor(GameEngine::renderer, 0xFF, 0xFF, 0xFF, 0x00);
	SDL_RenderClear(GameEngine::renderer);

  
    Draw_FilledHexagon(GameEngine::renderer, mPos, 250.f, { 1.f, 0.5f, 0.3f, 1.f });
	Draw_Hexagon(GameEngine::renderer, mPos, 300.f, { 255, 100, 128, 255 });

    SDL_SetRenderDrawColor(GameEngine::renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    Draw_Triangle(GameEngine::renderer,
        { mPos.x, mPos.y - 200.f },
        { mPos.x - 173.2f, mPos.y + 100.f },
		{ mPos.x + 173.2f, mPos.y + 100.f });

    Draw_FilledTriangle(GameEngine::renderer,
        { mPos.x, mPos.y - 150.f },
        { mPos.x - 129.9f, mPos.y + 75.f },
		{ mPos.x + 129.9f, mPos.y + 75.f },
		{ 0.0f, 1.0f, 0.5f, 1.0f });

    SDL_SetRenderDrawColor(GameEngine::renderer, 0x00, 0xFF, 0x80, 0xFF);
    Draw_Circle(GameEngine::renderer, width / 2, height / 2, width / 3);

    SDL_SetRenderDrawColor(GameEngine::renderer, 0x00, 0x80, 0xFF, 0xFF);
    Draw_FilledCircle(GameEngine::renderer, width / 2, height / 2, width / 20);

    //GenerateGradient();
    //ApplyBlur(2);

    // Render the logo onto the morph texture
    SDL_SetTextureBlendMode(logoTexture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(GameEngine::renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_SetTextureAlphaMod(logoTexture, 200);
    SDL_FRect destRect = { (width - 200.f) / 2.f, (height - 100.f) / 2.f, 200.f, 100.f };
    SDL_RenderTexture(GameEngine::renderer, logoTexture, nullptr, &destRect);



     // Restore main render target
    SDL_SetRenderTarget(GameEngine::renderer, nullptr);
}

void ColorMorphComponent::Render()
{
    // Copy our texture onto the main renderer
    SDL_RenderTexture(GameEngine::renderer, morphTexture, nullptr, nullptr);
}

void ColorMorphComponent::GenerateGradient()
{
    // Fill texture with interpolated colors based on color points
    const int steps = 6;

    SDL_SetTextureBlendMode(morphTexture, SDL_BLENDMODE_BLEND);
    for (auto& cp : points) {
        SDL_SetRenderDrawColor(GameEngine::renderer, cp.color.r, cp.color.g, cp.color.b, 220);
        int radius = width / steps;
        //SDL_FRect circleRect = { cp.x * width - radius / 2, cp.y * height - radius / 2, float(radius), float(radius) };
        //SDL_RenderFillRect(GameEngine::renderer, &circleRect);
		Draw_FilledCircle(GameEngine::renderer, static_cast<int>(cp.x * width), static_cast<int>(cp.y * height), radius);
    }
}

void ColorMorphComponent::ApplyBlur(int passes)
{
    // Simple blur: multiple passes with semi-transparent overlays
    SDL_SetTextureBlendMode(morphTexture, SDL_BLENDMODE_ADD);
    for (int i = 0; i < passes; ++i) {
        SDL_SetTextureAlphaMod(morphTexture, 180 - i * 40);
        SDL_RenderTexture(GameEngine::renderer, morphTexture, nullptr, nullptr);
    }
}