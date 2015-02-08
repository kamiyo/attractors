#include "Generator.h"

#include <algorithm>
#include <functional>
#include <fstream>
#include <random>
#include <iostream>

const double Generator::EPSILON = 1e-9;
const double Generator::MIN_COEFF = -10, Generator::MAX_COEFF = 10;


void Generator::search() {
	std::cout << "order: " << O << std::endl;
	while (1) {
		reset();
		getCoeff();
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
	std::cout << "n_points: " << x0.size() << std::endl;
}

void Generator::plot(int prev) {
	prev = prev - 1;
	std::ofstream out("attractor.txt");
	for (int i = prev; i < xn.size(); i++) {
		out << x0[i - prev] << " " << xn[i] << std::endl;
	}
	out.flush();
	out.close();
}

void Generator::reset() {

	current_x = first_x;
	x0.clear();
	xn.clear();
	r = lsum = L = 0.;
	N = NL = 0;
	if (O < 2) {
		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<int> dis(2, MAX_ORDER + 1);
		O = dis(gen);
	}
	coeff.clear();
	coeff.resize(O+1);
}

void Generator::getCoeff() {
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_real_distribution<double> dis(MIN_COEFF, MAX_COEFF);
	auto rand = std::bind(dis, std::ref(gen));
	std::generate(coeff.begin(), coeff.end(), rand);
}

int Generator::iterate() {
	while (1) {
		x0.push_back(current_x);
		double tempx = current_x;
		current_x = 0;
		double x_pow_accum = 1;
		for (int i = 0; i < coeff.size(); i++) {
			current_x += coeff[i] * x_pow_accum;
			x_pow_accum *= tempx;
		}
		xn.push_back(current_x);
		N++;
		if (abs(current_x) > 1e6) {
			return 3;
		}
		if (N < Generator::MIN_ITER) {
			continue;
		}
		if (N >= Generator::MAX_ITER) {
			return 1;
		}
		if (abs(current_x - tempx) < Generator::EPSILON) {
			return 0;
		}
		lyapunov();
		if (N >= MIN_ITER && L < 0.005) {
			return 2;
		}
	}
}