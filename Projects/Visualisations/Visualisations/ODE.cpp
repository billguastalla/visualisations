#include "ODE.h"
#include <imgui/imgui.h>
#include <glm/glm.hpp>

void Trajectory::AttractorODE_Lorenz::drawUI()
{
	glm::vec3 fsigRb{ (float)sigma,(float)R,(float)b };
	ImGui::SliderFloat3("Lorentz {Sigma,R,B}", &fsigRb[0], 0.0f, 30.f);
	if (
		fsigRb[0] != sigma ||
		fsigRb[1] != R ||
		fsigRb[2] != b)
	{
		sigma = fsigRb[0];
		R = fsigRb[1];
		b = fsigRb[2];
	}
}

void Trajectory::AttractorODE_RabinovichFabrikant::drawUI()
{
	glm::vec2 fAlphaGamma{ (float)alpha,(float)gamma};
	ImGui::SliderFloat2("RabinovichFabrikant {Alpha,Gamma}", &fAlphaGamma[0], 0.0f, 30.f);
	if (
		fAlphaGamma[0] != alpha ||
		fAlphaGamma[1] != gamma)
	{
		alpha = fAlphaGamma[0];
		gamma = fAlphaGamma[1];
	}
}

void Trajectory::AttractorODE_Rossler::drawUI()
{
	glm::vec3 fabc{ (float)a_r,(float)b_r,(float)c_r };
	ImGui::SliderFloat3("Rossler {a,b,c}", &fabc[0], 0.0f, 30.f);
	if (
		fabc[0] != a_r ||
		fabc[1] != b_r ||
		fabc[2] != c_r)
	{
		a_r = fabc[0];
		b_r = fabc[1];
		c_r = fabc[2];
	}
}

void Trajectory::AttractorODE_Multiscroll::drawUI()
{
	glm::vec4 fabc{ (float)a_r,(float)b_r,(float)c_r,(float)u_r };
	ImGui::SliderFloat4("Multiscroll {a,b,c,u}", &fabc[0], 0.0f, 30.f);
	if (
		fabc[0] != a_r ||
		fabc[1] != b_r ||
		fabc[2] != c_r ||
		fabc[3] != u_r)
	{
		a_r = fabc[0];
		b_r = fabc[1];
		c_r = fabc[2];
		u_r = fabc[3];
	}
}
