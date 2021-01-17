//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//  This file is an original or modified version of code provided by Joey De Vries. //
//                                                                                  //
//  It is licensed under the CC BY-NC 4.0 license. You can find                     //
//  a copy of this license at the top of the project tree.                          //
//                                                                                  //
//  LearnOpenGL:  https://learnopengl.com/                                          //
//  Github:       https://github.com/JoeyDeVries/                                   //
//  Website:      https://joeydevries.com/                                          //
//  Twitter:      https://twitter.com/JoeyDeVriez                                   //
//                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////



//#include "Shader.h"
//#include <fstream>
//#include <sstream>
//#include <iostream>
//
//Shader::Shader(const GLchar * vertexPath, const GLchar * fragmentPath)
//{
//	std::string vertexCode, fragmentCode;
//	std::ifstream vShaderFile, fShaderFile;
//
//	/* Ifstream objects can throw exceptions. */
//	vShaderFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
//	fShaderFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
//
//	try
//	{
//		/* Open files */
//		vShaderFile.open(vertexPath);
//		fShaderFile.open(fragmentPath);
//		std::stringstream vShaderStream, fShaderStream;
//		/* Read file's buffer contents into streams */
//		vShaderStream << vShaderFile.rdbuf();
//		fShaderStream << fShaderFile.rdbuf();
//		/* Close file handlers. */
//		vShaderFile.close();
//		fShaderFile.close();
//		/* Convert stream into string */
//		vertexCode = vShaderStream.str();
//		fragmentCode = fShaderStream.str();
//	}
//	catch (std::ifstream::failure e)
//	{
//		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
//	}
//	const char * vShaderCode = vertexCode.c_str();
//	const char * fShaderCode = fragmentCode.c_str();
//
//	unsigned int vertex, fragment;
//	int success;
//	char infoLog[512];
//
//	/* Vertex Shader */
//	vertex = glCreateShader(GL_VERTEX_SHADER);
//	glShaderSource(vertex, 1, &vShaderCode, nullptr);
//	glCompileShader(vertex);
//	/* Print Vertex Shader Compile Errors: */
//	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
//	if (!success)
//	{
//		glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
//		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << infoLog << std::endl;
//	}
//
//	/* Fragment Shader */
//	fragment = glCreateShader(GL_FRAGMENT_SHADER);
//	glShaderSource(fragment, 1, &fShaderCode, nullptr);
//	glCompileShader(fragment);
//	/* Print Vertex Shader Compile Errors: */
//	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
//	if (!success)
//	{
//		glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
//		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << infoLog << std::endl;
//	}
//
//	m_program = glCreateProgram();
//	glAttachShader(m_program, vertex);
//	glAttachShader(m_program, fragment);
//	glLinkProgram(m_program);
//
//	glGetProgramiv(m_program, GL_LINK_STATUS, &success);
//	if (!success)
//	{
//		glGetProgramInfoLog(m_program, 512, nullptr, infoLog);
//		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
//	}
//
//	glDeleteShader(vertex);
//	glDeleteShader(fragment);
//}
//
//void Shader::use()
//{
//	glUseProgram(m_program);
//}
//
//void Shader::setBool(const std::string & name, bool value) const
//{
//	unsigned int uniformLocation = glGetUniformLocation(m_program, name.c_str());
//	glUniform1i(uniformLocation, (int)value);
//}
//
//void Shader::setInt(const std::string & name, int value) const
//{
//	unsigned int uniformLocation = glGetUniformLocation(m_program, name.c_str());
//	glUniform1i(uniformLocation, value);
//}
//
//void Shader::setFloat(const std::string & name, float value) const
//{
//	unsigned int uniformLocation = glGetUniformLocation(m_program, name.c_str());
//	glUniform1f(uniformLocation, value);
//}
