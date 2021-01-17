#version 330 core
layout (location = 0) in vec2 texQuad;

out vec2 TexCoords;
out vec4 ParticleColour;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 cameraRight;
uniform vec3 cameraUp;

uniform vec4 colour;
uniform vec3 position;
uniform float scale;

void main()
{
    vec3 vertexWorldSpace = position + (cameraRight * texQuad.x * scale) + (cameraUp * texQuad.y * scale);
    TexCoords = texQuad;
    ParticleColour = colour;
    gl_Position = projection * view * vec4(vertexWorldSpace,1.0);
}

