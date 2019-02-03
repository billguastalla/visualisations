#pragma once
class Buffer;

class Visualisation
{
public:
	Visualisation() : m_active{ false } {};
	
	/* Set up shaders, initial VAOs, VBOs */
	virtual void activate() = 0;
	/* Unload shaders etc. */
	virtual void deactivate() = 0;

	virtual void processSamples(const Buffer & buf, unsigned samples) = 0;
	virtual void renderFrame() = 0;

protected:
	bool m_active;
};