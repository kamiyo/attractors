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
		double dx = current_x - temp_x;
		double dy = current_x - temp_y;
		double d2 = dx * dx + dy * dy;
		if (d2 > 0) {
			double df = 1e12 * d2;
			double rs = 1 / sqrt(df);
			if (df > 0) {
				lsum += log(df);
				NL++;
			}
			L = lsum / NL;
		}
	}

	void getCoeff();
	void plot(int prev);

	std::vector<Vector2d> xy;
	std::queue<double> buff;
	std::vector<double> coeffx, coeffy;
	VectorXd pows;
	double first_x, first_y;
	double current_x, current_y;
	double temp_x, temp_y;
	double lsum;
	int n_coeff;
	int O;
	int N;
	int NL;
	double L;
};
