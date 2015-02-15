#include <Eigen/Core>
#include <Eigen/StdVector>
#include <Eigen/Geometry>
#include <iostream>
#include "Generator1D.h"
#include "Generator2D.h"
#include <tclap/CmdLine.h>
#include <Windows.h>

int main(int argc, char** argv) {

	int dim, ord, prev;
	double x, y, z;

	try {
		TCLAP::CmdLine cmd("Generate Strange Attractors", ' ');
		TCLAP::ValueArg<int> _dim("d", "dimension", "the dimension of the attractor", true, 1, "int", cmd);
		TCLAP::ValueArg<int> _ord("o", "order", "the order of the attractor polynomial", false, 0, "int", cmd);
		TCLAP::ValueArg<double> _x("x", "initial_x", "the initial values of the iterator", true, 0.5, "initial x coord", cmd);
		TCLAP::ValueArg<double> _y("y", "initial_y", "the initial values of the iterator", false, 0.5, "initial y coord", cmd);
		TCLAP::ValueArg<double> _z("z", "initial_z", "the initial values of the iterator", false, 0.5, "initial z coord", cmd);
		TCLAP::ValueArg<int> _prev("p", "previous", "nth previous iterate to plot", false, 1, "int", cmd);
		cmd.parse(argc, argv);

		dim = _dim.getValue();
		ord = _ord.getValue();
		prev = _prev.getValue();
		x = _x.getValue();
		y = _y.getValue();
		z = _z.getValue();
	}
	catch (TCLAP::ArgException &e) {
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}

	Generator* g;
	if (dim == 1) {
		g = new Generator1D(x, ord);
	}
	else if (dim == 2) {
		g = new Generator2D(x, y, ord);
	}
	else {
		g = new Generator1D(0, 0);
	}
	g->search();
	g->plot(prev);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

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

	return 0;
}