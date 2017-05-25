#include "buildFragmentShader.hpp"
#include<iostream>
GLuint buildFragmentShader(const char* shaderSrc) {
	GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShaderID, 1, (const GLchar**)&shaderSrc, NULL);
	glCompileShader(fragShaderID);
	GLint isCompiled = 0;
	glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(fragShaderID, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(fragShaderID, maxLength, &maxLength, &errorLog[0]);
		for (int i = 0; i < maxLength; i++)
			cout << errorLog[i];
		glDeleteShader(fragShaderID); // Don't leak the shader.
		std::getchar();
		return NULL;
	}
	else {
		cout << "\n COMPILATION SUCCESSFULL \n";
	}
	return fragShaderID;
}