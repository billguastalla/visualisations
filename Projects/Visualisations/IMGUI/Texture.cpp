#include "Texture.h"

#include <glad/glad.h>
Texture TextureGenerator::loadTexture(std::vector<unsigned char> d, int width, int height, int nChannels)
{
	Texture result{};
	glGenTextures(1, &result.id);
	glBindTexture(GL_TEXTURE_2D, result.id);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* data = &d[0];
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	return result;
}

std::vector<unsigned char> TextureGenerator::noise(int width, int height, int channels)
{
	int len = width * height * channels;
	std::vector<unsigned char> out{};
	out.resize(len);
	for (int i = 0; i < len; ++i)
		out[i] = (unsigned char)(rand() % 128);
	return out;
}

std::string Texture::typeStr(Type t)
{
	switch (t)
	{
	case Texture::Type::Diffuse:
		return "texture_diffuse";
	case Texture::Type::Specular:
		return "texture_specular";
	case Texture::Type::Normal:
		return "texture_normal";
	case Texture::Type::Height:
		return "texture_height";
	default:
		return "unknown";
	}
}
