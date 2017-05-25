#include "Shader.hpp"

Shader::Shader(GLuint fragmentShader, GLuint vertexShader) {
	this->fragmentShader = fragmentShader;
	this->vertexShader = vertexShader;
	this->shaderProgram = buildShaderProgram(fragmentShader, vertexShader);
}

GLuint Shader::setFragmentShader(GLuint fragmentShader) {
	this->fragmentShader = fragmentShader;
	this->shaderProgram = buildShaderProgram(fragmentShader, vertexShader);
	return this->fragmentShader;
}

GLuint Shader::setVertexShader(GLuint vertexShader) {
	this->vertexShader = vertexShader;
	this->shaderProgram = buildShaderProgram(fragmentShader, vertexShader);
	return this->vertexShader;
}

GLuint Shader::getFragmentShader() {
	return this->fragmentShader;
}

GLuint Shader::getVertexShader() {
	return this->vertexShader;
}

GLuint Shader::getShaderProgram() {
	return this->shaderProgram;
}