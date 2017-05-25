#pragma once
#include "superHeader.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
class VertexObject
{
private:
	GLfloat* vertices;
	GLuint* indices;
	GLfloat* texCoords;
	GLuint vao;
	GLuint vbo;
	GLuint texCoordVb;
	GLuint vertID;
	GLuint textureCoordID;
	GLuint indxBufID;
	GLuint NUM_VERTICES;
	GLuint NUM_INDICES;
	GLuint NUM_TEXCOORDS;
	Shader *shader;
	vec3 position;
	vec4 rotation;
public:
	VertexObject(GLfloat *vertices, GLfloat *texCoords, GLuint *indices, GLuint NUM_VERTICES, GLuint NUM_TEXCOORDS, GLuint NUM_INDICES);
	~VertexObject();

	GLuint getVertID();
	GLuint getVAO();
	GLuint getVBO();
	GLuint getShader();
	vec3 setPosition(vec3 position);
	vec3 getPosition();
	vec4 setRotation(vec4 rotationVector);
	vec4 getRotation();
	void render(Camera camera);
	void attachShaderProgram(Shader *shaderID);

	GLuint setVertID(GLuint vertID);
};

