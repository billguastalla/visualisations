//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//  This file is an original or modified version of code provided by Joey De Vries. //
//                                                                                  //
//  It is licensed under the CC BY-NC 4.0 license. You can find                     //
//  a copy of this license at the top of the project tree.                          //
//                                                                                  //
//  LearnOpenGL:  https://learnopengl.com/                                          //
//  Github:       https://github.com/JoeyDeVries/                                   //
//  Website:      https://joeydevries.com/                                          //
//  Twitter:      https://twitter.com/JoeyDeVriez                                   //
//                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////
#version 330 core
layout (location = 0) out vec4 FragColour;
layout (location = 1) out vec4 BrightColor;

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

	float specularStrength = 0.7;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),32);
	vec3 specular = specularStrength * spec * lightColour;
	
	float diff = max(dot(norm, lightDir),0.0);
	vec3 diffuse = diff * lightColour;

	float ambientStrength = 0.2;
	vec3 ambient = ambientStrength * lightColour;
	
	vec3 result = (ambient + diffuse + specular) * objectColour;
    FragColour = vec4(result, 1.0);
	
    // check whether result is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(result.xyz, vec3(0.3426, 0.3452, 0.3722));
    if(brightness > 1.0)
        BrightColor = vec4(result, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
