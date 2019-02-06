#pragma once
#include <memory>

class Settings_Visualisation;

class Controller_Visualisation
{
public:
	Controller_Visualisation(std::shared_ptr<Settings_Visualisation> & settings);
	~Controller_Visualisation();


private:
	std::shared_ptr<Settings_Visualisation> m_settings;
};

