#version 330 core

// When OpenGL sees these two layout locations, it will write corresponding variables to each indexed
// colourbuffer, in one render pass.
layout (location = 0) out vec4 FragColour;
layout (location = 1) out vec4 BrightColour;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColour;
uniform vec3 lightColour;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	// LIGHTING: Specular, Diffuse, Ambient lighting
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),32);
	vec3 specular = specularStrength * spec * lightColour;
	
	float diff = max(dot(norm, lightDir),0.0);
	vec3 diffuse = diff * lightColour;

	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColour;
	
	vec3 result = (ambient + diffuse + specular) * objectColour;
    FragColour = vec4(result, 1.0);
	
	// BLOOM: Threshold for brightness, used to extract bright features of image.
	float brightness = dot(FragColour.rgb,vec3(0.2126,0.7152,0.0722));
	if(brightness > 1.0)
		BrightColour = vec4(FragColour.rgb,1.0);
	else
		BrightColour = vec4(0.0,0.0,0.0,0.0);
}
