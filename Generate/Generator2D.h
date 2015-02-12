#pragma once

#include "Generator.h"

typedef Eigen::Vector2d Vector2d;

#pragma once
class Generator2D : public Generator{
public:

	Generator2D(double initx, double inity, int order = 0)
		: first_x(initx)
		, first_y(inity)
		, O(order)
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
	void plot(int prev);

	std::vector<Vector2d> xy;
	std::queue<double> buff;
	std::vector<double> coeffx, coeffy;
	VectorXd pows, dx;
	double first_x, first_y;
	double current_x, current_y;
	double lsum;
	int n_coeff;
	int O;
	int N;
	int NL;
	double L;
};
