#include <Eigen/Core>
#include <Eigen/StdVector>
#include <Eigen/Geometry>
#include "Generator.h"
#include <sstream>
#include <tclap/CmdLine.h>
#include <Windows.h>
#undef near
#undef far

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include "glfunctions.h"

namespace {
	GLFWwindow* glWindow;
	Camera* camera;
}

void error_cb(int error, const char* description) {
	std::cerr << "error " << error << ": " << description << std::endl;;
}

void bufferSize_cb(GLFWwindow* glWindow, int width, int height) {
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	camera->updateCamera(width, height);
}

static void key_cb(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}
}

int GLinit(int w, int h) {
	glfwSetErrorCallback(error_cb);
	if (!glfwInit()) {
		return 1;
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
	glWindow = glfwCreateWindow(w, h, "Attractor", NULL, NULL);
	if (!glWindow) {
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(glWindow);
	glfwSetFramebufferSizeCallback(glWindow, bufferSize_cb);
	glfwSetKeyCallback(glWindow, key_cb);
	glewExperimental = GL_TRUE;
	glewInit();

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	std::cout << "renderer: " << renderer << std::endl;
	std::cout << "OpenGL version: " << version << std::endl;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	return 0;
}

int initBuffers(std::vector<GLuint> buffers, int n) {

}



int main(int argc, char** argv) {
	int dim, ord, prev;
	double x, y, z, iter;
	std::string inFile, iterString;
	try {
		TCLAP::CmdLine cmd("Generate Strange Attractors", ' ');
		TCLAP::ValueArg<int> _dim("d", "dimension", "the dimension of the attractor", false, 1, "int", cmd);
		TCLAP::ValueArg<int> _ord("o", "order", "the order of the attractor polynomial", false, 0, "int", cmd);
		TCLAP::ValueArg<double> _x("x", "initial_x", "the initial values of the iterator", false, iINF, "initial x coord", cmd);
		TCLAP::ValueArg<double> _y("y", "initial_y", "the initial values of the iterator", false, iINF, "initial y coord", cmd);
		TCLAP::ValueArg<double> _z("z", "initial_z", "the initial values of the iterator", false, iINF, "initial z coord", cmd);
		TCLAP::ValueArg<int> _prev("p", "previous", "nth previous iterate to plot", false, 1, "int", cmd);
		TCLAP::ValueArg<std::string> _inFile("f", "coeff_file", "binary file of 1 byte header (4 bits order, 4 bits dim) + doubles", false, "", "file name", cmd);
		TCLAP::ValueArg<std::string> _iter("i", "iterations", "number of iterations, can be sci notation", false, "", "int or sci", cmd);
		cmd.parse(argc, argv);

		dim = _dim.getValue();
		ord = _ord.getValue();
		prev = _prev.getValue();
		x = _x.getValue();
		y = _y.getValue();
		z = _z.getValue();
		inFile = _inFile.getValue();
		iterString = _iter.getValue();
		istringstream iss(iterString);
		iss >> iter;
	}
	catch (TCLAP::ArgException &e) {
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}

	Generator* g;
	if (inFile != "") {
		if (iterString == "") {
			g = new Generator(inFile);
		}
		else {
			g = new Generator(inFile, (int) iter);
		}
		if (x != iINF) {
			if (y != iINF) {
				if (z != iINF) {
					g->setInitial(x, y, z);
				}
				else {
					g->setInitial(x, y);
				}
			}
			else {
				g->setInitial(x);
			}
		}
		g->generate();
	}
	else {
		if (dim == 1) {
			g = new Generator(x, ord);
		}
		else if (dim == 2) {
			g = new Generator(x, y, ord);
		}
		else if (dim == 3) {
			g = new Generator(x, y, z, ord);
		}
		else {
			std::cerr << "error: dim fail" << std::endl;
			return 0;
		}
		g->search();
	}
	g->storePoints();

	GLinit(800, 600);

	if (inFile == "") {
		std::cout << "Save coefficients? [Yy/Nn] ";
		char save;
		std::cin >> save;
		if (save == 'y' || save == 'Y') {
			std::cout << "File name: ";
			std::string fn;
			std::cin >> fn;
			g->storeCoeff(fn);
		}
	}

	delete g;

	return 0;
}


/*
STARTUPINFO si;
PROCESS_INFORMATION pi;
ZeroMemory(&si, sizeof(si));
si.cb = sizeof(si);
ZeroMemory(&pi, sizeof(pi));
if (dim == 2) {
if (!CreateProcess(NULL,
"python plot_data.py",
NULL,
NULL,
FALSE,
0,
NULL,
NULL,
&si,
&pi))
{
std::cout << "Couldn't run plot_data.py" << std::endl;
}

WaitForSingleObject(pi.hProcess, INFINITE);
CloseHandle(pi.hProcess);
CloseHandle(pi.hThread);
}
else if (dim == 3) {
if (!CreateProcess(NULL,
"python plot_vpython.py",
NULL,
NULL,
FALSE,
0,
NULL,
NULL,
&si,
&pi))
{
std::cout << "Couldn't run plot_data.py" << std::endl;
}

WaitForSingleObject(pi.hProcess, INFINITE);
CloseHandle(pi.hProcess);
CloseHandle(pi.hThread);

}*/