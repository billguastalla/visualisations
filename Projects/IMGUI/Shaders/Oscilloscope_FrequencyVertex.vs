#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;

out vec3 fragPos;
out vec3 colourAttrib;

void main()
{
	fragPos = aPos;
	colourAttrib = aColour;
	gl_Position = vec4(aPos,1.0);
}