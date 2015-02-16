#pragma once

#include <array>
#include <fstream>
#include <iostream>
#include <queue>
#include <vector>
#include <Eigen/core>

typedef Eigen::Vector2d Vector2d;
typedef Eigen::Vector3d Vector3d;
typedef Eigen::VectorXd VectorXd;
typedef Eigen::VectorXi VectorXi;
typedef Eigen::ArrayXd ArrayXd;
typedef Eigen::MatrixXd MatrixXd;
typedef Eigen::MatrixXi MatrixXi;

class Generator {
public:
	const static int MAX_ITER = (int)1e6;
	const static int MIN_ITER = (int)5e2;
	const static int MAX_ORDER = 5;	// O+1 coeff
	const static double EPSILON;
	const static double MIN_COEFF, MAX_COEFF;

	virtual void reset() = 0;
	virtual void search() = 0;
	virtual int iterate() = 0;
	virtual Vector2d step() = 0;
	virtual void lyapunov() = 0;
	virtual void genCoeff() = 0;
	virtual void getCoeff(std::ofstream& o) = 0;
	void storeCoeff() {
		try {
			std::ofstream out("coeff.dat", std::ios::out | std::ios::binary);
			unsigned char dim = D - 1; // < 3 (uses 2 bits)
			unsigned char order = O << 4;
			unsigned char head = dim | order;
			out.write((char*) &head, sizeof(char));
			getCoeff(out);
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
			int dim = head & 0x3;
			if (dim != D) {
				throw;
			}
			int order = head >> 4;
			std::cout << dim << " " << order << std::endl;
		}
		catch (std::exception e) {
			std::cerr << e.what() << std::endl;
		}
	}

	virtual void storePoints() = 0;

	int n_coeff;
	int O;
	int D;
};

bool operator< (const std::pair<double, double>& lh, const std::pair<double, double>& rh);
std::ostream& operator<< (std::ostream& o, const std::vector<std::pair<double, double> >& p);
std::ostream& operator<< (std::ostream& o, const Vector2d& v);
