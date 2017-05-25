#pragma once
#include "superHeader.hpp"

class Texture {
public:
	Texture(string filename);
	Texture::Texture(int width, int height, int depth, bool depthTexture);
	~Texture();
	void bind(GLuint unit);
	GLuint getTextureID();
protected:
private:
	GLuint m_texture;

};