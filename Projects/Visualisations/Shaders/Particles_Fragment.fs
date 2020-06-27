#version 330 core
in vec2 TexCoords;
in vec4 ParticleColour;
out vec4 colour;

uniform sampler2D sprite;

void main()
{
	colour = (texture(sprite, TexCoords) * ParticleColour);
}