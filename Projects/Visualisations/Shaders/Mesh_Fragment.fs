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
out vec4 FragColour;

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;

uniform vec3 objectColour;
uniform vec3 lightColour;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

void main()
{
	//vec3 texPix = texture(texture_diffuse1, TexCoord).xyz;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	float specularStrength = 0.7;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),32);
	vec3 specular = specularStrength * spec * lightColour * vec3(texture(texture_specular1, TexCoord));
	
	float diff = max(dot(norm, lightDir),0.0);
	vec3 diffuse = diff * lightColour * vec3(texture(texture_diffuse1, TexCoord));

	float ambientStrength = 0.35;
	vec3 ambient = ambientStrength * lightColour;
	
	vec3 result = (ambient + diffuse + specular) * objectColour;
    FragColour = vec4(result, 1.0);
}
