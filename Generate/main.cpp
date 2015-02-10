#include <Eigen/Core>
#include <Eigen/StdVector>
#include <Eigen/Geometry>
#include <iostream>
#include "Generator1D.h"
#include <tclap/CmdLine.h>
#include <Windows.h>

int main(int argc, char** argv) {

	int dim, ord, prev;
	std::vector<double> inits;

	try {
		TCLAP::CmdLine cmd("Generate Strange Attractors", ' ');
		TCLAP::ValueArg<int> _dim("d", "dimension", "the dimension of the attractor", true, 1, "int", cmd);
		TCLAP::ValueArg<int> _ord("o", "order", "the order of the attractor polynomial", false, 0, "int", cmd);
		TCLAP::MultiArg<double> _inits("x", "initial", "the initial values of the iterator", true, "# of dimension doubles", cmd);
		TCLAP::ValueArg<int> _prev("p", "previous", "nth previous iterate to plot", true, 1, "int", cmd);
		cmd.parse(argc, argv);

		dim = _dim.getValue();
		ord = _ord.getValue();
		prev = _prev.getValue();
		inits = _inits.getValue();
	}
	catch (TCLAP::ArgException &e) {
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}

	std::cout << dim << " " << ord << " " << prev << std::endl;
	for (double i : inits) {
		std::cout << i << " ";
	}
	std::cout << std::endl;

	Generator* g;
	if (dim == 1) {
		g = new Generator1D(inits[0], ord);
	}
	else {
		g = new Generator1D(atof(argv[2]), atoi(argv[3]));
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