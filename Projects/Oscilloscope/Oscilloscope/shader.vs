#version 330 core
layout (location = 0) in vec3 aGeometry;
layout (location = 1) in vec3 aColour;

uniform vec3 spaceshipPos;
out vec3 spaceshipColour;

void main()
{
	// Can probably swizzle here.
	gl_Position = vec4(aGeometry.x + spaceshipPos.x, aGeometry.y + spaceshipPos.y, aGeometry.z + spaceshipPos.z, 1.0);
	spaceshipColour = aColour;
}
