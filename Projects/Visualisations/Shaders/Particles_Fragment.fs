#version 330 core
in vec2 TexCoords;
in vec4 ParticleColour;
out vec4 colour;

uniform sampler2D sprite;

void main()
{
    vec4 texCol = texture(sprite, TexCoords);
	if(texCol.a < 0.1)
		discard;
	colour = texCol * ParticleColour;
}