#pragma once

class Settings_Visualisation
{
	enum class VisualisationSelection
	{
		Oscilloscope,
		Cubes
	};

public:
	Settings_Visualisation();
	~Settings_Visualisation();
	
private:
	VisualisationSelection m_visSelection;
};

