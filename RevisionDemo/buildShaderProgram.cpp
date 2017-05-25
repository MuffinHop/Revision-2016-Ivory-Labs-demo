#include "buildShaderProgram.hpp"

GLuint buildShaderProgram(GLuint vertShaderID, GLuint fragShaderID) {
	GLuint shaderID = glCreateProgram();
	glAttachShader(shaderID, vertShaderID);
	glAttachShader(shaderID, fragShaderID);
	glLinkProgram(shaderID);
	return shaderID;
}