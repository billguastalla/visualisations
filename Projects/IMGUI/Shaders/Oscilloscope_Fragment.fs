#version 330 core

out vec4 FragColour;
in vec3 fragPos;

void main()
{
	vec3 colour = vec3((fragPos.x + fragPos.y)/2.0,(fragPos.x - fragPos.y)/2.0,(fragPos.x * fragPos.y));
	FragColour = vec4(colour,1.0);
}