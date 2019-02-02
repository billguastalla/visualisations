#pragma once
#include "Visualisation.h"
class Buffer;

class Visualisation_Cubes : public Visualisation
{
public:
	Visualisation_Cubes();

	/* Set up shaders, initial VAOs, VBOs */
	void activate() override;
	/* Unload shaders etc. */
	void deactivate() override;

	void render(Buffer & buf) = 0;
};