#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <time.h>

#define GL_LOG_FILE "gl.log"

bool restart_gl_log() {
	FILE* file;
	fopen_s(&file, GL_LOG_FILE, "w+");
	if (!file) {
		fprintf(stderr, "error: could not open %s log file for writing\n", GL_LOG_FILE);
		return false;
	}
	time_t now = time(NULL);
	char date[26];
	ctime_s(date, 26, &now);
	fprintf(file, "%s log, local time %s\n", GL_LOG_FILE, date);
	fclose(file);
	return true;
}

bool gl_log(const char* message, const char* filename, int line) {
	FILE* file;
	fopen_s(&file, GL_LOG_FILE, "a+");
	if (!file) {
		fprintf(stderr, "error: could not open %s for writing\n", GL_LOG_FILE);
		return false;
	}
	fprintf(file, "%s:%i %s\n", filename, line, message);
	fclose(file);
	return true;
}

bool gl_error(const char* message, const char* filename, int line) {
	FILE* file;
	fopen_s(&file, GL_LOG_FILE, "a+");
	if (!file) {
		fprintf(stderr, "error: could not open %s for writing\n", GL_LOG_FILE);
		return false;
	}
	fprintf(file, "ERROR!!\n%s:%i %s\n", filename, line, message);
	fclose(file);
	return true;
}

void glfw_error_callback(int error, const char* description) {
	fputs(description, stderr);
	gl_log(description, __FILE__, __LINE__);
}

void log_gl_params() {
	GLenum params [] = {
		GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
		GL_MAX_CUBE_MAP_TEXTURE_SIZE,
		GL_MAX_DRAW_BUFFERS,
		GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
		GL_MAX_TEXTURE_IMAGE_UNITS,
		GL_MAX_TEXTURE_SIZE,
		GL_MAX_VARYING_FLOATS,
		GL_MAX_VERTEX_ATTRIBS,
		GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
		GL_MAX_VERTEX_UNIFORM_COMPONENTS,
		GL_MAX_VIEWPORT_DIMS,
		GL_STEREO,
	};
	const char* names [] = {
		"GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
		"GL_MAX_CUBE_MAP_TEXTURE_SIZE",
		"GL_MAX_DRAW_BUFFERS",
		"GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
		"GL_MAX_TEXTURE_IMAGE_UNITS",
		"GL_MAX_TEXTURE_SIZE",
		"GL_MAX_VARYING_FLOATS",
		"GL_MAX_VERTEX_ATTRIBS",
		"GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
		"GL_MAX_VERTEX_UNIFORM_COMPONENTS",
		"GL_MAX_VIEWPORT_DIMS",
		"GL_STEREO",
	};
	gl_log("GL Context Params:", __FILE__, __LINE__);
	char msg[256];
	// integers - only works if the order is 0-10 integer return types
	for (int i = 0; i < 10; i++) {
		int v = 0;
		glGetIntegerv(params[i], &v);
		sprintf_s(msg, 256, "%s %i", names[i], v);
		gl_log(msg, __FILE__, __LINE__);
	}
	// others
	int v[2];
	v[0] = v[1] = 0;
	glGetIntegerv(params[10], v);
	sprintf_s(msg, 256, "%s %i %i\n", names[10], v[0], v[1]);
	gl_log(msg, __FILE__, __LINE__);
	unsigned char s = 0;
	glGetBooleanv(params[11], &s);
	sprintf_s(msg, 256, "%s %i\n", names[11], (unsigned int) s);
	gl_log(msg, __FILE__, __LINE__);
	gl_log("-----------------------------\n", __FILE__, __LINE__);
}

#endif