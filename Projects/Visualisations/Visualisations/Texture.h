#pragma once
#include <string>
#include <vector>

struct Texture {
	Texture()
		: id{ 0 }, path{ "" }, t{ Type::Diffuse } {}

	unsigned int id;
	enum class Type { Diffuse, Specular, Normal, Height} t;
	static std::string typeStr(Type t);
	std::string path;
};

class TextureGenerator
{
public:
	static Texture loadTexture(std::string filename);
	static Texture loadTexture(unsigned char* data, int w, int h, int c);
	static std::vector<unsigned char> noise(int width, int height, int channels);
};