#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;

void main()
{             
    vec3 textureValue = texture(scene, TexCoords).rgb;      
    FragColor = vec4(textureValue, 1.0);
}
