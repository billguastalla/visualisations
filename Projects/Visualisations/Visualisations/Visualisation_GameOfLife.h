#pragma once
#include "Visualisation.h"
#include "Camera.h"
#include "Mesh.h"

#include <deque>
#include <memory>
#include <vector>
class Buffer;
struct GLFWwindow;
class Shader;
class Game;

class Visualisation_GameOfLife : public Visualisation
{
public:
	Visualisation_GameOfLife();

	/* Set up callbacks, shaders, vertex objects */
	void activate() override;
	/* Free callbacks, shaders, vertex objects */
	void deactivate() override;

	void processSamples(const Buffer& buf, unsigned samples) override;
	void renderFrame() override;
	std::string titleString() override { return "N-D Game of Life"; };


	void drawInterface() override;
private:
	Mesh m_cube;

	Shader* m_objectShader;
	Shader* m_lampShader;

	std::vector<float> m_vertices;
	std::vector<glm::vec3> m_cubePositions;
	std::vector<glm::vec3> m_cubeColours;
	std::deque<double> m_signalPowerHistory;

	glm::vec3 m_lightPos;

	std::unique_ptr<Game> m_game;

	int m_frameLoop;
	std::vector<std::vector<std::vector<int>>> m_coordsHistory;

	float m_spacing;
	int m_reinitExtent;
	int m_frameUpdateRate;
};