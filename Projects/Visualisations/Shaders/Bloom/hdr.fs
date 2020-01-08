// HDR Fragment Shader
#version 330 core
out vec4 FragColour;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
// Whether hdr is enabled
uniform bool hdr;
// Exposure setting
uniform float exposure;

void main()
{
	const float gamma = 2.2;
	vec3 hdrColour = texture(hdrBuffer, TexCoords).rgb;
	if(hdr)
	{
		// reinhard:
		vec3 result = hdrColour / (hdrColour + vec3(1.0));
		//result = vec3(1.0) - exp(-hdrColour * exposure);
		// gamma correction
		result = pow(result, vec3(1.0 / gamma));
		FragColour = vec4(result, 1.0);
	}
	else
	{
		vec3 result = pow(hdrColour, vec3(1.0 / gamma));
		FragColour = vec4(result,1.0);
	}
}