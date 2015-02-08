
#include <iostream>
#include <vector>

#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfStringAttribute.h>
#include <OpenEXR/ImfMatrixAttribute.h>
#include <OpenEXR/ImfArray.h>

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include <Eigen/Core>
#include <Eigen/StdVector>
#include <Eigen/Geometry>

#include "glfunc.h"

GLFWwindow* glWindow;
typedef Eigen::Matrix3d Matrix3d;
typedef Eigen::Matrix4d Matrix4d;
typedef Eigen::Vector3d Vector3d;
typedef Eigen::Vector4d Vector4d;

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

// OpenEXR writing
void writeRgba(const char fileName [], const Imf::Rgba *pixels, int width, int height)
{
	Imf::RgbaOutputFile file(fileName, width, height, Imf::WRITE_RGBA);
	file.setFrameBuffer(pixels, 1, width);
	file.writePixels(height);
}

void updateCamera() {
	float zNear = 0.01, zFar = 500;
	//float d = (float) tracer.camera->_focalDistance;
	//float imageRight = (float) tracer.camera->_width / (2. * d);
	//float imageTop = (float) tracer.camera->_height / (2. * d);
	////camera = perspective(imageRight * zNear, zNear * imageTop, zNear, zFar).cast<float>();
	////camera *= lookat(tracer.camera->_e, tracer.camera->_dir, Vector3d(0, 1, 0)).cast<float>();
	////cout << tracer.camera->_e << " " << tracer.camera->_dir << endl;
	//camera = perspective(imageRight * zNear, imageTop * zNear, zNear, zFar).cast<float>();
	//camera *= lookat(tracer.camera->_ep, tracer.camera->_e2p, Vector3d(0, 1, 0)).cast<float>();
}

void bufferSize_cb(GLFWwindow* glWindow, int width, int height) {
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	updateCamera();
}

void error_cb(int error, const char* description) {
	cout << "error " << error << ": " << description << endl;
}

static void key_cb(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}
}

double points [] = {
	0.0, 0.5, 0.0,
	0.5, -0.5, 0.0,
	-0.5, -0.5, 0.0
};

double colours [] = {
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0
};


int main(int argc, char** argv) {
	/*Imf::Array2D<Imf::Rgba> output;
	output.resizeErase(800, 600);
	output[j][i].a = 1.0;
	if (i == (int) 3 * j / 4) {
	output[j][i].r = 0.0;
	output[j][i].g = 0.0;
	output[j][i].b = 0.0;
	}
	else {
	output[j][i].r = 1.0;
	output[j][i].g = 1.0;
	output[j][i].b = 1.0;
	}
	*/
	std::vector<double> vertices;
	std::vector<double> colors;

	for (unsigned int j = 0; j < 800; j++){
		for (unsigned int i = 0; i < 600; i++) {
			if (i == (int) 3 * j / 4) {
				vertices.push_back((double)j / 800);
				vertices.push_back((double)i / 600);
				vertices.push_back(0.0);
				colors.push_back(1.0);
				colors.push_back(1.0);
				colors.push_back(1.0);
			}
		}
	}
	std::cout << vertices.size() << std::endl;

	//writeRgba("out.exr", &output[0][0], 800, 600);

	int width = 800, height = 600;

	glfwSetErrorCallback(error_cb);
	if (!glfwInit()) {
		return 1;
	}
	string windowName = "Attractor";
	glfwWindowHint(GLFW_SAMPLES, 4);
	glWindow = glfwCreateWindow(800, 600, windowName.c_str(), NULL, NULL);
	if (!glWindow) {
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(glWindow);
	glfwSetWindowSizeCallback(glWindow, bufferSize_cb);
	glfwSetKeyCallback(glWindow, key_cb);

	glewExperimental = GL_TRUE;
	glewInit();

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "renderer: " << renderer << endl;
	cout << "OpenGL version: " << version << endl;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLuint pos_vbo = 0, color_vbo = 0;

	glGenBuffers(1, &pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(double), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(double), &colors[0], GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint shaderProgram = createProgram("../../Attractors/vert.glsl", "../../Attractors/frag.glsl");
	GLuint vp_loc = glGetAttribLocation(shaderProgram, "vp");
	GLuint color_loc = glGetAttribLocation(shaderProgram, "input_color");
	GLuint camera_loc = glGetUniformLocation(shaderProgram, "camera");
	//Matrix4d camera = Matrix4d::Identity();
	glClearColor(0.f, 0.f, 0.f, 1.0);

	bufferSize_cb(glWindow, width, height);

	while (!glfwWindowShouldClose(glWindow)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnableVertexAttribArray(vp_loc);
		glEnableVertexAttribArray(color_loc);
		glUseProgram(shaderProgram);
		//glUniformMatrix4dv(camera_loc, 1, GL_FALSE, camera.data());

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, pos_vbo);
		glVertexAttribPointer(vp_loc, 3, GL_DOUBLE, GL_FALSE, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
		glVertexAttribPointer(color_loc, 3, GL_DOUBLE, GL_FALSE, 0, NULL);
		glDrawArrays(GL_POINTS, 0, vertices.size() / 3);

		glfwSwapBuffers(glWindow);
		glfwWaitEvents();
	}

	return 0;
}