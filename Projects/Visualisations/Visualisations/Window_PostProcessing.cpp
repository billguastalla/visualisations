#include "Window_PostProcessing.h"
#include "PostProcessing.h"
#include <imgui/imgui.h>
#include <GLM/glm.hpp>

Window_PostProcessing::Window_PostProcessing(std::shared_ptr<PostProcessing>& pprocessing)
	:
	p_postProcessing{pprocessing}
{
}

Window_PostProcessing::~Window_PostProcessing()
{
}

void Window_PostProcessing::draw()
{
	ImGui::Begin(windowTitle().c_str());

	ImGui::Text("Post Processing:");
	/* Post-processing details: */
	bool wireframe = p_postProcessing->wireframe();
	ImGui::Checkbox("Wireframe", &wireframe);
	p_postProcessing->setWireframe(wireframe);

	bool hdrEnabled = p_postProcessing->HDREnabled();
	ImGui::Checkbox("HDR", &hdrEnabled);
	p_postProcessing->setHDREnabled(hdrEnabled);

	float exposure = p_postProcessing->exposure();
	ImGui::Text("Exposure");
	ImGui::SameLine();
	ImGui::SliderFloat("", &exposure, 0.0, 1.0);
	p_postProcessing->setExposure(exposure);

	/* Strange post-processing */
	glm::vec2 colBufPerc = p_postProcessing->colBufPerc();
	glm::vec2 colBufSlide{ colBufPerc };
	ImGui::SliderFloat2("Colour Buffer Offset", (float*)&colBufPerc, 0.9f, 1.0f);
	glm::vec2 rbPerc = p_postProcessing->rbPerc();
	glm::vec2 rbSlide{ rbPerc };
	ImGui::SliderFloat2("Render Buffer Offset", (float*)&rbPerc, 0.9f, 1.0f);
	glm::vec2 pingPongPerc = p_postProcessing->pingPongPerc();
	glm::vec2 pingPongSlide{ pingPongPerc };
	ImGui::SliderFloat2("Ping Pong Offset", (float*)&pingPongPerc, 0.9f, 1.0f);
	if (colBufPerc != colBufSlide)
		p_postProcessing->setColBufPerc(colBufPerc);
	if (rbPerc != rbSlide)
		p_postProcessing->setrbPerc(rbPerc);
	if (pingPongPerc != pingPongSlide)
		p_postProcessing->setpingpongPerc(pingPongPerc);

	ImGui::End();
}
