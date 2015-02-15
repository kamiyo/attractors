#pragma once

#include "Generator.h"

#pragma once
class Generator1D : public Generator{
public:

	Generator1D(double init, int order = 0, double ratio = 0)
		: first_x(init)
		, O(order)
		, r(ratio)
	{
		reset();
	}

	void reset();

	void search();
	int iterate();
	void lyapunov() {
		double df = (dx.cwiseProduct(xpow.tail(coeff.size() - 1))).dot(Eigen::Map<VectorXd>(coeff.data() + 1, coeff.size() - 1));
		df = abs(df);
		if (df > 0) {
			lsum += log(df);
			NL += 1;
		}
		L = lsum / NL;
		return;
	}

	void getCoeff();
	void storeCoeff();
	void storePoints();

	std::vector<double> xs, ys;
	std::queue<double> buff;
	std::vector<double> coeff;
	VectorXd xpow, dx;
	double first_x;
	double current_x;
	double r;
	double lsum;
	int O;
	int N;
	int NL;
	double L;
};
