#include "buildVertexShader.hpp"
GLuint buildVertexShader(const char* shaderSrc) {
	GLuint vertShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShaderID, 1, (const GLchar**)&shaderSrc, NULL);
	glCompileShader(vertShaderID);
	GLint isCompiled = 0;
	glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vertShaderID, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(vertShaderID, maxLength, &maxLength, &errorLog[0]);

		glDeleteShader(vertShaderID); // Do not leak the shader.
		return NULL;
	}
	else {
		cout << "\n COMPILATION SUCCESSFULL \n";
	}
	return vertShaderID;
}