#include <Eigen/Core>
#include <Eigen/StdVector>
#include <Eigen/Geometry>
#include "Generator.h"
#include <sstream>
#include <tclap/CmdLine.h>
#include <Windows.h>
#undef near
#undef far


#include <array>

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include "glfunctions.h"

namespace {
	GLFWwindow* glWindow;
	Matrix4d camera;
	Generator* g;
	Vector3d mid;
	double rad;
	bool leftmousedown = false;
	MatrixXd gui_points;
	MatrixXd gui_colors;
	std::array<GLuint, 2> gui_vbos;
	GLuint gui_vao;
	GLuint shaderProgram;
	GLuint vp_loc;
	GLuint camera_loc;
	GLuint color_loc;
}

void error_cb(int error, const char* description) {
	std::cerr << "error " << error << ": " << description << std::endl;;
}

void updateCamera() {
	if (g != nullptr) {
		if (g->D == 2) {
			camera = Camera::orthographic(g->min.x(), g->max.x(), g->max.y(), g->min.y(), 1, -1);
		}
		else if (g->D == 3) {
			mid = (g->min + g->max) / 2;
			std::cout << mid << std::endl;
			rad = (g->max - mid).norm();
			std::cout << "firstrad: " << std::endl;
			double dist = rad / tan(M_PI / 6.0);
			std::cout << tan(M_PI / 6.0) << std::endl;
			camera = Camera::perspective(0.1, 0.075, 0.1, 10);
			camera *= Camera::lookat(mid - dist * mid.normalized(), mid, Vector3d(0, 1, 0));
			std::cout << mid - dist * mid.normalized() << std::endl;
		}
	}
}

void mouseButton_cb(GLFWwindow* w, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			leftmousedown = true;
		}
		else {
			leftmousedown = false;
		}
	}
}

void bufferSize_cb(GLFWwindow* w, int width, int height) {
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	updateCamera();
}

void key_cb(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}
}

void display_gui() {
	if (leftmousedown) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glBindVertexArray(gui_vao);
		glDrawArrays(GL_LINES, 0, gui_points.cols());
		glBindVertexArray(0);
		glDisable(GL_DEPTH_TEST);
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
	//glfwSetInputMode(glWindow, GLFW_STICKY_MOUSE_BUTTONS, 1);
	glfwSetMouseButtonCallback(glWindow, mouseButton_cb);
	glewExperimental = GL_TRUE;
	glewInit();

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	std::cout << "renderer: " << renderer << std::endl;
	std::cout << "OpenGL version: " << version << std::endl;

	glEnable(GL_MULTISAMPLE);
	glClearColor(0, 0, 0, 0);

	shaderProgram = createProgram("vert.glsl", "frag.glsl");

	vp_loc = glGetAttribLocation(shaderProgram, "vp");
	camera_loc = glGetUniformLocation(shaderProgram, "camera");
	color_loc = glGetAttribLocation(shaderProgram, "color");

	updateCamera();

	gui_points.resize(3, 6);
	gui_points.colwise() = mid;
	std::cout << "rad: " << rad << std::endl;
	gui_points.col(1) += rad * Vector3d(1., 0., 0.);
	gui_points.col(3) += rad * Vector3d(0., 1., 0.);
	gui_points.col(5) += rad * Vector3d(0., 0., 1.);

	gui_colors.resize(3, 6);
	gui_colors.col(0) << 1., 0., 0.;
	gui_colors.col(1) = gui_colors.col(0);
	gui_colors.col(2) << 0., 1., 0.;
	gui_colors.col(3) = gui_colors.col(2);
	gui_colors.col(4) << 0., 0., 1.;
	gui_colors.col(5) = gui_colors.col(4);

	glGenVertexArrays(1, &gui_vao);
	glBindVertexArray(gui_vao);
	
	glGenBuffers(2, &gui_vbos[0]);
	
	glBindBuffer(GL_ARRAY_BUFFER, gui_vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, gui_points.size() * sizeof(double), gui_points.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(vp_loc, 3, GL_DOUBLE, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(vp_loc);

	glBindBuffer(GL_ARRAY_BUFFER, gui_vbos[1]);
	glBufferData(GL_ARRAY_BUFFER, gui_colors.size() * sizeof(double), gui_colors.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(color_loc, 3, GL_DOUBLE, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(color_loc);

	glBindVertexArray(0);

	return 0;
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
	
	GLuint vao;
	std::array<GLuint, 2> vbos;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(2, &vbos[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	MatrixXd data(g->points);
	if (g->D == 2) {
		data.conservativeResize(3, Eigen::NoChange);
		data.row(2).setZero();
	}
	
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(double), data.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(vp_loc);
	glVertexAttribPointer(vp_loc, 3, GL_DOUBLE, GL_FALSE, 0, NULL);
	
	MatrixXd colors; colors.resize(data.rows(), data.size());
	colors.setConstant(0.2);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(double), colors.data(), GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 3, GL_DOUBLE, GL_FALSE, 0, NULL);

	glEnable(GL_BLEND);
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	glBlendFunc(GL_ONE, GL_ONE);
	updateCamera();

	glBindVertexArray(0);
	
	while (!glfwWindowShouldClose(glWindow)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glUniformMatrix4dv(camera_loc, 1, GL_FALSE, camera.data());
		glBindVertexArray(vao);
		glDrawArrays(GL_POINTS, 0, data.cols());

		display_gui();
		
		glUseProgram(0);

		glfwSwapBuffers(glWindow);
		glfwWaitEvents();
	}

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