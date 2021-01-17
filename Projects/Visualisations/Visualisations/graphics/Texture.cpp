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
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
Texture TextureGenerator::loadTexture(std::string filename)
{
	Texture result{};
	int texWidth{ 0 }, texHeight{ 0 }, texColourChannels{ 0 };
	unsigned char* texData = stbi_load(filename.c_str(), &texWidth, &texHeight, &texColourChannels, 0);
	if (texData != nullptr)
	{
		result = loadTexture(texData, texWidth, texHeight, texColourChannels);
		result.path = filename;
		stbi_image_free(texData);
	}
	return result;
}

Texture TextureGenerator::loadTexture(unsigned char * data, int width, int height, int nChannels)
{
	Texture result{};
	glGenTextures(1, &result.id);
	glBindTexture(GL_TEXTURE_2D, result.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, nChannels == 4 ? GL_RGBA : GL_RGB, width, height, 0, nChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
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
