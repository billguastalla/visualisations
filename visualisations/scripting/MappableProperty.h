/* DRAFT */
#pragma once
#include "../camera/CameraSystem.h"
#include <vector>
/* Not used yet, this is a draft of a design to allow mapping of properties in a scripted sequence */
class EverythingThatCanChangeAValue;

class Mapping;
class MappingSystem
{
	void calculateAndApplyValues(EverythingThatCanChangeAValue*, double t);

	void ptree_load();
	void ptree_save();
private:
	std::vector<std::pair<double, MappableProperty>> calculate(EverythingThatCanChangeAValue*, double t);
	void apply(std::vector< std::pair<double, MappableProperty>>, EverythingThatCanChangeAValue*, double t);

	std::vector<Mapping> m_mappables;
};

class MappableProperty;
class Mapping // e.g. {AudioTrack1.Amplitude maps to Scene.Object1.Size with Exponential.c1=0.5,c2=0.3 from time t_0=5s to time t_f=10s)
{
private:
	std::pair<MappableProperty /*out*/, MappableProperty /*in*/> m_properties;
	Interpolation m_interpolation;
};

class MappableProperty // 
{
	enum class ObjectType { Audio, Scene, PostProcess };

};