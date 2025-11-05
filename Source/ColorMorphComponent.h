#pragma once
#include "ObjectComponent.h"
#include <SDL3/SDL.h>
#include <vector>

class ColorMorphComponent : public ObjectComponent
{
	public:
	static bool FactoryRegistered;
	static ObjectComponent* Create();

	ColorMorphComponent();
	virtual ~ColorMorphComponent();

	virtual ComponentType GetComponentType() const override { return ComponentType::Visual; }

	virtual void Process() override;
	virtual void Render() override;
	virtual void Initialize();  // Initialize data and parameters

	virtual void OnEvent(const Message& msg) override;

	//---------------------------------------------------------
	// Specific ColorMorphComponent data
	SDL_Texture* morphTexture = nullptr;

	int width = 800, height = 600;
	float time = 0.f;

	struct ColorPoint {
		float x, y;          // Position in [0, 1] range
		SDL_Color color;     // Color at this point
	};
	std::vector<ColorPoint> points;

	void GenerateGradient(); // Fill the texture with interpolated colors
	void ApplyBlur(int passes = 2); // Apply soft blur

};

