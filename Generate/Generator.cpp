#include "Generator.h"
#include <iostream>

const double Generator::EPSILON = 1e-6;
const double Generator::MIN_COEFF = -2.0, Generator::MAX_COEFF = 2.0;
int Generator::MAX_ITER = (int)1e6;
int Generator::MIN_ITER = (int)5e2;
int Generator::MAX_ORDER = 5;	// O+1 coeff

bool operator< (const std::pair<double, double>& lh, const std::pair<double, double>& rh) {
	return (lh.first < rh.first);
}

std::ostream& operator<< (std::ostream& o, const std::vector<std::pair<double, double> >& p) {
	for (auto i : p) {
		o << i.first << " " << i.second << std::endl;
	}
	return o;
}

std::ostream& operator<< (std::ostream& o, const Vector2d& v) {
	return o << v.x() << " " << v.y();
}

void Generator::search() {
	std::cout << "order: " << O << std::endl;
	std::cout << "initial: " << std::endl;
	std::cout << initial << std::endl;
	int fix = 0, limit = 0, unb = 0;
	initialize();
	while (1) {
		reset();
		genCoeff();
		int result = iterate();
		if (result == 0) {
			fix++;
		//std::cout << "fixed point" << std::endl;
		}
		else if (result == 1) {
			std::cout << std::endl << "strange attractor" << std::endl;
			break;
		}
		else if (result == 2) {
		//std::cout << "limit cycle" << std::endl;
			limit++;
		}
		else if (result == 3) {
			unb++;
		// unbounded
		}
		if ((fix + limit + unb) % (int)1e6 == 0)
		std::cout << "\rfixed: " << fix << " limit: " << limit << " unbounded: " << unb;
	}
	std::cout << "coeffs:" << std::endl;
	std::cout << coeff << std::endl;
	std::cout << "iter: " << N << std::endl;
	std::cout << "lyapunov: " << L << std::endl;
}

void Generator::storePoints() {
	try {
		std::ofstream out("attractor.dat", std::ios::out | std::ios::binary);
		out.write(reinterpret_cast<char*>(points.data()), sizeof(double) * D * Generator::MAX_ITER);
		out.flush();
		out.close();
	}
	catch (std::exception e) {
		std::cerr << e.what() << std::endl;
	}

}

void Generator::reset() {
	save = e = last = current = initial;
	e[0] += 1e-6;
	lsum = L = 0.;
	N = NL = 0;
}

void Generator::genCoeff() {
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_real_distribution<double> dis(MIN_COEFF, MAX_COEFF);
	auto rand = std::bind(dis, std::ref(gen));
	std::generate(coeff.data(), coeff.data() + (n_coeff * D), rand);
}

VectorXd Generator::step() {

	// powers of x, y, z up to O-th power
	powList.row(0).setOnes();
	for (int i = 1; i < O + 1; i++) {
		powList.row(i) = powList.row(i - 1).cwiseProduct(current.transpose());
	}
	// permutation of powers
	powPerm.setOnes();
	for (int i = 0; i < n_coeff; i++) {
		for (int d = 0; d < D; d++) {
			powPerm(i) *= powList(permutation(i, d), d);
		}
	}

	return (powPerm.transpose() * coeff);
}

int Generator::iterate() {
	while (1) {
		last = current;
		current = step();
		points.col(N) = current;
		min = min.cwiseMin(current);
		max = max.cwiseMax(current);
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