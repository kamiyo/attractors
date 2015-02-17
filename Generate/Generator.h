#pragma once

#include <cassert>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
#include <vector>
#include <Eigen/core>

typedef Eigen::Matrix<double, 1, 1> Vector1d;
typedef Eigen::Vector2d Vector2d;
typedef Eigen::Vector3d Vector3d;
typedef Eigen::Vector4d Vector4d;
typedef Eigen::VectorXd VectorXd;
typedef Eigen::VectorXi VectorXi;
typedef Eigen::ArrayXd ArrayXd;
typedef Eigen::MatrixXd MatrixXd;
typedef Eigen::MatrixXi MatrixXi;

namespace {
	int INF = std::numeric_limits<int>::infinity();
}

class Generator {
public:
	static int MAX_ITER;
	static int MIN_ITER;
	static int MAX_ORDER; // O+1 coeff
	const static double EPSILON;
	const static double MIN_COEFF, MAX_COEFF;

	Generator(double initx, double inity, double initz, double initw, int order = 0)
		: initial(Vector4d(initx, inity, initz, initw))
		, O(order)
		, D(4)
	{
		reset();
	}

	Generator(double initx, double inity, double initz, int order = 0)
		: initial(Vector3d(initx, inity, initz))
		, O(order)
		, D(3)
	{
		reset();
	}

	Generator(double initx, double inity, int order = 0)
		: initial(Vector2d(initx, inity))
		, O(order)
		, D(2)
	{
		reset();
	}

	Generator(double initx, int order = 0)
		: O(order)
		, D(1)
	{
		initial.resize(1);
		initial << initx;
		reset();
	}

	Generator(const std::string& file, int iter = Generator::MAX_ITER) {
		Generator::MAX_ITER = iter;
		readCoeff(file);
	}

	void setInitial(double x, double y) {
		assert(D == 2);
		initial << x, y;
	}
	
	void setInitial(double x) {
		assert(D == 1);
		initial << x;
	}

	void setInitial(double x, double y, double z) {
		assert(D == 3);
		initial << x, y, z;
	}

	void generate() {
		for (int i = 0; i < Generator::MAX_ITER; i++) {
			current = step();
			points.col(i) = current;
		}
		std::cout << "initial:" << std::endl;
		std::cout << initial << std::endl;
		std::cout << "coeffs:" << std::endl;
		std::cout << coeff << std::endl;
		std::cout << "iter: " << Generator::MAX_ITER << std::endl;
		std::cout << "lyapunov: " << L << std::endl;
	}

	void reset();

	void search();
	int iterate();
	VectorXd step();

	VectorXi incrementPermutation(VectorXi in, int max, int size, int& sum) {
		in[0]++;
		sum++;
		if (sum > max) {
			in[0] = 0;
			sum = in.sum();
			in.tail(size - 1) = incrementPermutation(in.tail(size - 1), max, size - 1, sum);
		}
		return in;
	}

	MatrixXi createPermutation() {
		MatrixXi perm;
		perm.resize(n_coeff, D);
		int sum = 0;
		perm.row(0).setZero();
		for (int i = 1; i < perm.rows(); i++) {
			perm.row(i) = incrementPermutation(perm.row(i - 1), O, D, sum);
		}
		return perm;
	}

	void lyapunov() {
		save = current;
		current = e;
		VectorXd dx = step() - save;
		double d2 = dx.squaredNorm();

		if (d2 > 0) {
			double df = 1e12 * d2;
			double rs = 1.0 / sqrt(df);
			e = save + rs * dx;
			current = save;
			if (df > 0) {
				lsum += log(df);
				NL++;
			}
			L = lsum / NL;
		}
	}

	void lyapunov1D() {/*
		double df = (dx.cwiseProduct(xpow.tail(coeff.size() - 1))).dot(Eigen::Map<VectorXd>(coeff.data() + 1, coeff.size() - 1));
		df = abs(df);
		if (df > 0) {
			lsum += log(df);
			NL += 1;
		}
		L = lsum / NL;
		return;*/
	}

	void genCoeff();

	void storeCoeff() {
		try {
			std::ofstream out("coeff.dat", std::ios::out | std::ios::binary);
			unsigned char dim = D - 1; // 0 - 3 (uses 2 bits) rep. 1D - 4D
			unsigned char order = O << 4;
			unsigned char head = dim | order;
			out.write((char*) &head, sizeof(char));
			out.write(reinterpret_cast<char*>(initial.data()), sizeof(double) * D);
			out.write(reinterpret_cast<char*>(coeff.data()), sizeof(double) * n_coeff * D);
			out.write(reinterpret_cast<char*>(&L), sizeof(double));
			out.flush();
			out.close();
		}
		catch (std::exception e) {
			std::cerr << e.what() << std::endl;
		}
	}

	void readCoeff(const std::string& name) {
		try {
			std::ifstream in(name, std::ios::in | std::ios::binary);
			char head;
			in.read(&head, sizeof(char));
			D = head & 0x3;
			D += 1;
			O = head >> 4;
			std::cout << D << " " << O << std::endl;
			initial.resize(D);
			in.read(reinterpret_cast<char*>(initial.data()), sizeof(double) * D);
			reset();
			in.read(reinterpret_cast<char*>(coeff.data()), sizeof(double) * n_coeff * D);
			in.read(reinterpret_cast<char*>(&L), sizeof(double));
			in.close();
		}
		catch (std::exception e) {
			std::cerr << e.what() << std::endl;
		}
	}

	void storePoints();

	MatrixXd points;
	MatrixXd coeff;
	MatrixXd powList;
	MatrixXi permutation;
	VectorXd powPerm;
	VectorXd initial;
	VectorXd current;
	VectorXd last;
	VectorXd e, save;
	double lsum;
	int N;
	int NL;
	double L;
	int n_coeff;
	int O;
	int D;
};

bool operator< (const std::pair<double, double>& lh, const std::pair<double, double>& rh);
std::ostream& operator<< (std::ostream& o, const std::vector<std::pair<double, double> >& p);
std::ostream& operator<< (std::ostream& o, const Vector2d& v);
