#pragma once
#include "Visualisation.h"
#include "Shader.h"

class Visualisation_ODE : public Visualisation
{
	Visualisation_ODE();
public:
	void activate() override;
	void deactivate() override;

	void processSamples(const Buffer& buf, unsigned samples) override;
	void renderFrame() override;

	std::string titleString() override { return "ODEs/Harmonics"; };

	void drawInterface() override;


	Shader* m_particleShader;
};

