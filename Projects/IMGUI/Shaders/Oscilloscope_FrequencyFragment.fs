#version 330 core

out vec4 FragColour;
in vec3 fragPos;
in vec3 colourAttrib;

void main()
{
	//vec3 colour = vec3((fragPos.x + fragPos.y)/2.0,(fragPos.x - fragPos.y)/2.0,(fragPos.x * fragPos.y));
	FragColour = vec4(colourAttrib,1.0);
}