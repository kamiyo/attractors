#pragma once

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "gllog.h"
#include "typedefs.h"

class Camera {
public:
	Matrix4d perspective(double right, double top, double near, double far) {
		Matrix4d m; m << near / right, 0, 0, 0,
			0, near / top, 0, 0,
			0, 0, -(far + near) / (far - near), -2 * far * near / (far - near),
			0, 0, -1, 0;
		return m;
	}

	Matrix4d lookat(const Vector3d& eye, const Vector3d& dir, const Vector3d& up) {
		Vector3d u = dir.normalized().cross(up.normalized()).normalized();
		Vector3d v = u.cross(dir.normalized());
		Vector3d w = -dir.normalized();
		Matrix4d m = Matrix4d::Identity();
		m << u(0), u(1), u(2), -u.dot(eye),
			v(0), v(1), v(2), -v.dot(eye),
			w(0), w(1), w(2), -w.dot(eye),
			0, 0, 0, 1;
		return m;
	}

	Matrix4d orthographic(double left, double right, double top, double bottom, double near, double far) {
		Matrix4d m = Matrix4d::Identity();
		m.diagonal() << 2.0 / (right - left), 2.0 / (top - bottom), -2.0 / (far - near), 1.0;
		m.col(3) << -1.0 * (right + left) / (right - left), -1.0 * (top + bottom) / (top - bottom), -1.0 * (far + near) / (far - near), 1.0;
		return m;
	}

	void updateCamera(int width, int height) {
	}

	Vector3d _eye, _dir, _up;
	double _left, _right, _top, _bottom, _near, _far;
};

char* fileToChar(const char* filename) {
	long length;
	std::ifstream in(filename, std::ios::in | std::ios::binary);
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
