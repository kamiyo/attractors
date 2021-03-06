#include "Generator1D.h"

#include <algorithm>
#include <functional>
#include <fstream>
#include <random>
#include <iostream>

const double Generator::EPSILON = 1e-9;
const double Generator::MIN_COEFF = -10, Generator::MAX_COEFF = 10;

void Generator1D::search() {
	std::cout << "order: " << O << std::endl;
	while (1) {
		reset();
		genCoeff();
		int result = iterate();
		if (result == 0) {
			std::cout << "fixed point" << std::endl;
		}
		else if (result == 1) {
			std::cout << "strange attractor" << std::endl;
			break;
		}
		else if (result == 2) {
			std::cout << "limit cycle" << std::endl;
		}
		else if (result == 3) {
			continue;
			// unbounded
		}
	}
	std::cout << "coeff: [ ";
	for (double d : coeff) {
		std::cout << d << " ";
	}
	std::cout << "]" << std::endl;
	std::cout << "iter: " << N << std::endl;
	std::cout << "lyapunov: " << L << std::endl;
	std::cout << "n_points: " << xs.size() << std::endl;
}

void Generator1D::storePoints() {
	std::ofstream out("attractor.txt");
	for (int i = 0; i < ys.size(); i++) {
		out << xs[i] << " " << ys[i] << std::endl;
	}
	out.flush();
	out.close();
}

void Generator1D::reset() {

	current_x = first_x;
	xs.clear();
	xs.reserve(MAX_ITER);
	ys.clear();
	ys.reserve(MAX_ITER);
	r = lsum = L = 0.;
	N = NL = 0;
	if (O < 2) {
		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<int> dis(2, MAX_ORDER + 1);
		O = dis(gen);
	}
	n_coeff = O + 1;
	coeff.clear();
	coeff.resize(n_coeff);
	dx.setLinSpaced(O, 1, O);
}

void Generator1D::genCoeff() {
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_real_distribution<double> dis(MIN_COEFF, MAX_COEFF);
	auto rand = std::bind(dis, std::ref(gen));
	std::generate(coeff.begin(), coeff.end(), rand);
}

Vector2d Generator1D::step() {
	xs.push_back(current_x);
	prev_x = current_x;
	double x_pow_accum = 1;
	xpow.resize(n_coeff);
	for (int i = 0; i < n_coeff; i++) {
		xpow[i] = x_pow_accum;
		x_pow_accum *= prev_x;
	}
	return Vector2d(current_x, xpow.dot(Eigen::Map<VectorXd>(coeff.data(), coeff.size())));
}

int Generator1D::iterate() {
	while (1) {
		Vector2d result = step();
		xs.push_back(result.x());
		ys.push_back(result.y());
		N++;
		if (abs(current_x) > 1e6) {
			return 3;
		}
		if (N < Generator::MIN_ITER) {
			continue;
		}
		if (N >= Generator::MAX_ITER) {
			lyapunov();
			return 1;
		}
		if (abs(current_x - prev_x) < Generator::EPSILON) {
			return 0;
		}
		lyapunov();
		if (N >= MIN_ITER && L < 0.005) {
			return 2;
		}
	}
}