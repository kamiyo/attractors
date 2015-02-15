#include "Generator2D.h"

#include <algorithm>
#include <functional>
#include <fstream>
#include <random>
#include <iostream>

void Generator2D::search() {
	std::cout << "order: " << O << std::endl;
	while (1) {
		reset();
		getCoeff();
		int result = iterate();
		/*if (result == 0) {
			std::cout << "fixed point" << std::endl;
		}
		else*/
		if (result == 1) {
			std::cout << "strange attractor" << std::endl;
			break;
		}
		/*
		else if (result == 2) {
			std::cout << "limit cycle" << std::endl;
		}
		else if (result == 3) {
			continue;
			// unbounded
		}*/
	}
	std::cout << "x coeffs: [ ";
	for (double d : coeffx) {
		std::cout << d << " ";
	}
	std::cout << "]" << std::endl;
	std::cout << "y coeffs: [ ";
	for (double d : coeffy) {
		std::cout << d << " ";
	}
	std::cout << "]" << std::endl;
	std::cout << "iter: " << N << std::endl;
	std::cout << "lyapunov: " << L << std::endl;
	std::cout << "n_points: " << xy.size() << std::endl;
}

void Generator2D::storeCoeff() {
	/*try {
		std::ofstream out("coeff.dat", std::ios::out | std::ios::binary);
		for ()
	}*/
}

void Generator2D::storePoints() {
	try {
		std::ofstream out("attractor.dat", std::ios::out | std::ios::binary);
		for (Vector2d v : xy) {
			out.write((char*) &v, sizeof(Vector2d));
		}
		out.flush();
		out.close();
	}
	catch (std::exception e) {
		std::cerr << e.what() << std::endl;
	}
	
}

void Generator2D::reset() {

	save = e = last = current = initial;
	xy.clear();
	xy.reserve(MAX_ITER);
	lsum = L = 0.;
	N = NL = 0;
	if (O < 2) {
		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<int> dis(2, MAX_ORDER + 1);
		O = dis(gen);
	}
	n_coeff = (O + 1) * (O + 2) / 2;
	coeffx.clear(); coeffy.clear();
	coeffx.resize(n_coeff);
	coeffy.resize(n_coeff);
	pows.resize(n_coeff);
}

void Generator2D::getCoeff() {
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_real_distribution<double> dis(MIN_COEFF, MAX_COEFF);
	auto rand = std::bind(dis, std::ref(gen));
	std::generate(coeffx.begin(), coeffx.end(), rand);
	std::generate(coeffy.begin(), coeffy.end(), rand);
}

Vector2d Generator2D::step() {
	VectorXd powx, powy;
	powx.resize(O + 1); powy.resize(O + 1);
	powx[0] = current.x();
	powy[0] = current.y();
	for (int i = 1; i < O + 1; i++) {
		powx[i] = powx[i - 1] * current.x();
		powy[i] = powy[i - 1] * current.y();
	}
	int cur = 0;
	for (int i = 0; i < O + 1; i++) {
		for (int j = 0; (i + j) < O + 1; j++) {
			pows[cur++] = powx[j] * powy[i];
		}
	}

	return Vector2d(pows.dot(Eigen::Map<VectorXd>(coeffx.data(), coeffx.size()))
		, pows.dot(Eigen::Map<VectorXd>(coeffy.data(), coeffy.size())));
}

int Generator2D::iterate() {
	while (1) {
		last = current;
		current = step();
		xy.push_back(current);
		N++;
		if (current.cwiseAbs().sum() > 1e6) {
			return 3;
		}
		if (N < Generator::MIN_ITER) {
			continue;
		}
		if (N >= Generator::MAX_ITER) {
			lyapunov();
			return 1;
		}
		if ((current - last).cwiseAbs().sum() < Generator::EPSILON) {
			return 0;
		}
		lyapunov();
		if (N >= MIN_ITER && L < 0.005) {
			return 2;
		}
	}
}