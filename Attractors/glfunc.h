#ifndef __GLFUNC_H__
#define __GLFUNC_H__

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "log.h"

using namespace std;

char* fileToChar(const char* filename) {
	long length;
	ifstream in(filename, std::ios::in | std::ios::binary);
	if (in) {
		in.seekg(0, std::ios::end);
		length = in.tellg();
		char* buffer = new char[length + 1];
		in.seekg(0, std::ios::beg);
		in.read(buffer, length);
		in.close();
		buffer[length] = 0;
		return buffer;
	}
	throw(errno);
}

void _print_shader_info_log(GLuint shader) {
	GLint logSize = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
	char* log = (char*) malloc(sizeof(char) * logSize);
	glGetShaderInfoLog(shader, logSize, NULL, log);
	gl_log(log, __FILE__, __LINE__);
}

GLuint createShader(char* shad, GLenum type) {
	const char* shader = fileToChar(shad);
	GLuint s = glCreateShader(type);
	glShaderSource(s, 1, &shader, NULL);
	glCompileShader(s);
	int params = -1;
	glGetShaderiv(s, GL_COMPILE_STATUS, &params);
	_print_shader_info_log(s);
	if (GL_TRUE != params) {
		fprintf_s(stderr, "error: GL shader index %i did not compile\n", s);
		glDeleteShader(s);
		exit(0);
	}
	return s;
}

GLuint createProgram(char* vert, char* frag) {
	GLuint vs = createShader(vert, GL_VERTEX_SHADER);
	GLuint fs = createShader(frag, GL_FRAGMENT_SHADER);
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, fs);
	glAttachShader(shaderProgram, vs);
	glLinkProgram(shaderProgram);
	return shaderProgram;
}






#endif