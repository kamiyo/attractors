#include "Generator2D.h"

#include <algorithm>
#include <functional>
#include <fstream>
#include <random>
#include <iostream>

const double Generator::EPSILON = 1e-9;
const double Generator::MIN_COEFF = -10, Generator::MAX_COEFF = 10;

void Generator2D::search() {
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
	std::cout << "n_points: " << xs.size() << std::endl;
}

void Generator2D::plot(int prev) {
	prev = prev - 1;
	std::ofstream out("attractor.txt");
	for (int i = prev; i < ys.size(); i++) {
		out << xs[i - prev] << " " << ys[i] << std::endl;
	}
	out.flush();
	out.close();
}

void Generator2D::reset() {

	current_x = first_x;
	current_y = first_y;
	xs.clear();
	xs.reserve(MAX_ITER);
	ys.clear();
	ys.reserve(MAX_ITER);
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
	//!!!! dx.setLinSpaced(O, 1, O);
}

void Generator2D::getCoeff() {
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_real_distribution<double> dis(MIN_COEFF, MAX_COEFF);
	auto rand = std::bind(dis, std::ref(gen));
	std::generate(coeffx.begin(), coeffx.end(), rand);
	std::generate(coeffy.begin(), coeffy.end(), rand);
}

int Generator2D::iterate() {
	while (1) {
		pows.resize(n_coeff);
		VectorXd powx, powy;
		powx.resize(O + 1); powy.resize(O + 1);
		powx[0] = current_x;
		powy[0] = current_y;
		for (int i = 1; i < O + 1; i++) {
			powx[i] = powx[i - 1] * current_x;
			powy[i] = powy[i - 1] * current_y;
		}
		int cur = 0;
		for (int i = 0; i < O + 1; i++) {
			for (int j = 0; (i + j) < O + 1; j++) {
				pows[cur++] = powx[j] * powy[i];
			}
		}

		current_x = pows.dot(Eigen::Map<VectorXd>(coeffx.data(), coeffx.size()));
		current_y = pows.dot(Eigen::Map<VectorXd>(coeffy.data(), coeffy.size()));

		xy.push_back(Vector2d(current_x, current_y));
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
		if (abs(current_x - tempx) < Generator::EPSILON) {
			return 0;
		}
		lyapunov();
		if (N >= MIN_ITER && L < 0.005) {
			return 2;
		}
	}
}