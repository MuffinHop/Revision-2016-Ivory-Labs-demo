#pragma once
#include "superHeader.hpp"
#include "buildVertexShader.hpp"
#include "buildFragmentShader.hpp"
#include "buildShaderProgram.hpp"
#include "readShader.hpp"

class Shader {
private:
	GLuint fragmentShader;
	GLuint vertexShader;
	GLuint shaderProgram;

public:
	Shader(GLuint fragmentShader, GLuint vertexShader);

	GLuint setFragmentShader(GLuint fragmentShader);
	GLuint setVertexShader(GLuint vertexShader);

	GLuint getFragmentShader();
	GLuint getVertexShader();
	GLuint getShaderProgram();
};