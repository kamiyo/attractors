#include "Generator2D.h"

#include <algorithm>
#include <functional>
#include <random>
#include <iostream>

void Generator2D::search() {
	std::cout << "order: " << O << std::endl;
	while (1) {
		reset();
		genCoeff();
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
	}/*
	std::cout << "x coeffs: [ ";
	for (double d : coeffx) {
		std::cout << d << " ";
	}
	std::cout << "]" << std::endl;
	std::cout << "y coeffs: [ ";
	for (double d : coeffy) {
		std::cout << d << " ";
	}*/
	std::cout << "]" << std::endl;
	std::cout << "iter: " << N << std::endl;
	std::cout << "lyapunov: " << L << std::endl;
	//std::cout << "n_points: " << xy.size() << std::endl;
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
		/*for (Vector2d v : xy) {
			out.write((char*) &v, sizeof(Vector2d));
		}
		out.flush();*/
		out.close();
	}
	catch (std::exception e) {
		std::cerr << e.what() << std::endl;
	}
	
}

void Generator2D::reset() {
	save = e = last = current = initial;
	points.resize(D, MAX_ITER);
	lsum = L = 0.;
	N = NL = 0;
	if (O < 2) {
		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<int> dis(2, MAX_ORDER + 1);
		O = dis(gen);
	}
	n_coeff = 1;
	double denom = 1;
	for (int i = 1; i <= D; i++) {
		n_coeff *= O + i;
		if (i > 1) {
			denom *= i - 1;
		}
	}
	n_coeff /= denom * D;
	coeff.resize(n_coeff, D);
	pows.resize(n_coeff, D);
	permutation = createPermutation();
}

void Generator2D::genCoeff() {
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_real_distribution<double> dis(MIN_COEFF, MAX_COEFF);
	auto rand = std::bind(dis, std::ref(gen));
	std::generate(coeff.data(), coeff.data() + (n_coeff * D), rand);
}

VectorXd Generator2D::step() {
	MatrixXd pow;
	pow.resize(O + 1, D);
	// powers of x, y, z up to O-th power
	pow.row(0).setOnes();
	for (int i = 1; i < O + 1; i++) {
		pow.row(i) = pow.row(i - 1).cwiseProduct(current.transpose());
	}
	// permutation of powers
	pows.setOnes();
	for (int i = 0; i < n_coeff; i++) {
		for (int d = 0; d < D; d++) {
			pows(i, d) *= pow(permutation(i, d), d);
		}
	}
	std::cout << "before multiply by coeffs" << std::endl;
	std::cout << pows << std::endl;
	std::cout << "coeffs" << std::endl;
	std::cout << coeff << std::endl;
	VectorXd temp; temp.resize(D);
	for (int d = 0; d < D; d++) {
		temp(d) = coeff.col(d).dot(pows.col(d));
	}
	return temp;
}

int Generator2D::iterate() {
	while (1) {
		last = current;
		current = step();
		//xy.push_back(current);
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