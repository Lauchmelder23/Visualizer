#include "Shader.hpp"

#include <iostream>	
#include <glad/glad.h>	

AbstractShader::AbstractShader(const std::string& vertexShader, const std::string& fragmentShader) :
	id(0)
{
	GLint success;
	GLchar infoLog[512];

	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexShaderSource = vertexShader.c_str();
	glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShaderID);

	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderID, 512, NULL, infoLog);
		std::cerr << "Vertex shader creation failed: \n" << infoLog << std::endl;

		glDeleteShader(vertexShaderID);
		return;
	}

	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragmentShaderSource = fragmentShader.c_str();
	glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShaderID);

	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderID, 512, NULL, infoLog);
		std::cerr << "Fragment shader creation failed: \n" << infoLog << std::endl;

		glDeleteShader(fragmentShaderID);
		glDeleteShader(vertexShaderID);
		return;
	}

	id = glCreateProgram();
	glAttachShader(id, vertexShaderID);
	glAttachShader(id, fragmentShaderID);
	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(fragmentShaderID, 512, NULL, infoLog);
		std::cerr << "Shader program linking failed: \n" << infoLog << std::endl;

		glDeleteShader(fragmentShaderID);
		glDeleteShader(vertexShaderID);

		id = 0;

		return;
	}

	glDeleteShader(fragmentShaderID);
	glDeleteShader(vertexShaderID);
}

AbstractShader::~AbstractShader()
{
	glDeleteProgram(id);
}

void AbstractShader::Use()
{
	glUseProgram(id);
}
