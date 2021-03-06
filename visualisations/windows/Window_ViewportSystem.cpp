
#include "Window_ViewportSystem.h"
#include "../models/Model_ViewportSystem.h"
#include "../camera/CameraSystem.h"
#include <imgui.h>

Window_ViewportSystem::Window_ViewportSystem(std::shared_ptr<Model_ViewportSystem>& viewport)
	: m_viewportSystem{ viewport }
{

}

Window_ViewportSystem::~Window_ViewportSystem()
{

}

void Window_ViewportSystem::draw()
{
	ImGui::Begin(windowTitle().c_str());

	bool f{ m_viewportSystem->freeCamera() };
	ImGui::Checkbox("Free Camera", &f);
	m_viewportSystem->setFreeCamera(f);

	glm::vec3 cPos = m_viewportSystem->camera().m_position;
	Geometry::YawPitchRoll _ypr{ m_viewportSystem->camera().m_orientation };
	ImGui::Text("\tFramerate: %.3f ms/frame (%.1f fps)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text(std::string{ "Camera Position = {" +
		std::to_string(cPos.x).substr(0,5) + ", " +
		std::to_string(cPos.y).substr(0,5) + ", " +
		std::to_string(cPos.z).substr(0,5) + "} " }.c_str());

	ImGui::Text(std::string{ "Camera Yaw/Pitch/Roll = {" +
		std::to_string(_ypr.x).substr(0,5) + ", " +
		std::to_string(_ypr.y).substr(0,5) + ", " +
		std::to_string(_ypr.z).substr(0,5) + "} " }.c_str());

	
	ImGui::Text("Perspective");
	float log10zNear{ std::log10(m_viewportSystem->camera().m_nearZ) };
	float log10zFar{ std::log10(m_viewportSystem->camera().m_farZ) };
	float zoom{ m_viewportSystem->camera().m_zoom };
	ImGui::SliderFloat("log10[zNear]", &log10zNear, -10.f, 0.f);
	ImGui::SliderFloat("log10[zFar]", &log10zFar, 0.f, 10.f);
	ImGui::SliderFloat("zoom",&zoom,0.f,90.f);
	if (
		std::abs(log10zNear - std::log10(m_viewportSystem->camera().m_nearZ)) > 0.01f ||
		std::abs(log10zFar - std::log10(m_viewportSystem->camera().m_farZ)) > 0.01f ||
		zoom != m_viewportSystem->camera().m_zoom
		)
	{
		m_viewportSystem->camera().m_nearZ = std::powf(10, log10zNear);
		m_viewportSystem->camera().m_farZ = std::powf(10, log10zFar);
		m_viewportSystem->camera().m_zoom = zoom;
	}

	bool currentAsStart{ ImGui::Button("Set current camera as start") };
	m_viewportSystem->cameraSystem().drawUI();
	if (currentAsStart)
		m_viewportSystem->cameraSystem().setStartPos(m_viewportSystem->camera().position());
	//ImGui::Text("Post Processing:");
	///* Post-processing details: */
	//bool wireframe = m_visualisation->postProcessing()->wireframe();
	//ImGui::Checkbox("Wireframe", &wireframe);
	//m_visualisation->postProcessing()->setWireframe(wireframe);

	//bool hdrEnabled = m_visualisation->postProcessing()->HDREnabled();
	//ImGui::Checkbox("HDR", &hdrEnabled);
	//m_visualisation->postProcessing()->setHDREnabled(hdrEnabled);

	//float exposure = m_visualisation->postProcessing()->exposure();
	//ImGui::Text("Exposure");
	//ImGui::SameLine();
	//ImGui::SliderFloat("", &exposure, 0.0, 1.0);
	//m_visualisation->postProcessing()->setExposure(exposure);

	///* Strange post-processing */
	//glm::vec2 colBufPerc = m_visualisation->postProcessing()->colBufPerc();
	//glm::vec2 colBufSlide{ colBufPerc };
	//ImGui::SliderFloat2("Colour Buffer Offset", (float*)&colBufPerc, 0.9f, 1.0f);
	//glm::vec2 rbPerc = m_visualisation->postProcessing()->rbPerc();
	//glm::vec2 rbSlide{ rbPerc };
	//ImGui::SliderFloat2("Render Buffer Offset", (float*)&rbPerc, 0.9f, 1.0f);
	//glm::vec2 pingPongPerc = m_visualisation->postProcessing()->pingPongPerc();
	//glm::vec2 pingPongSlide{ pingPongPerc };
	//ImGui::SliderFloat2("Ping Pong Offset", (float*)&pingPongPerc, 0.9f, 1.0f);
	//if (colBufPerc != colBufSlide)
	//	m_visualisation->postProcessing()->setColBufPerc(colBufPerc);
	//if (rbPerc != rbSlide)
	//	m_visualisation->postProcessing()->setrbPerc(rbPerc);
	//if (pingPongPerc != pingPongSlide)
	//	m_visualisation->postProcessing()->setpingpongPerc(pingPongPerc);




	//std::string s{ "Options: " };
	//s += m_visualisation->currentVisualisation()->titleString();
	//s += ":";
	//ImGui::Text(s.c_str());
	//m_visualisation->currentVisualisation()->drawInterface();

	//m_visualisation->setVisualisation(sel);
	ImGui::End();
}
