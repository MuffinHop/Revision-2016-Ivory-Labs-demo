#include "Texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(string filename) {
	int width, height, numComponents;
	const char * f = filename.c_str();
	unsigned char *imageData = stbi_load(f, &width, &height, &numComponents, 4);

	if (imageData == NULL) {
		cerr << "Texture load fail : " << filename << " !!!!!!!!!!!!!!!!\n";
	}

	glGenTextures(1, &this->m_texture);

	glBindTexture(GL_TEXTURE_2D, this->m_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, imageData);


	stbi_image_free(imageData);
}
Texture::Texture(int width, int height, int depth, bool depthTexture) {
	glGenTextures(1, &this->m_texture);
	glBindTexture(GL_TEXTURE_2D, this->m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
Texture::~Texture() {
	glDeleteTextures(1, &this->m_texture);
}
void Texture::bind(GLuint unit) {
	if (this->m_texture == NULL) return;
	assert(unit >= 0 && unit <= 31);
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, this->m_texture);
}
GLuint Texture::getTextureID() {
	return this->m_texture;
}