#pragma once

#include "Generator.h"

#pragma once
class Generator2D : public Generator{
public:

	Generator2D(double initx, double inity, int order = 0)
		: initial(Vector2d(initx, inity))
		, O(order)
	{
		reset();
	}

	void reset();

	void search();
	int iterate();
	Vector2d step();

	void fractal() {

	}

	void lyapunov() {
		save = current;
		current = e;
		Vector2d next = step();
		Vector2d dx = next - save;
		double d2 = dx.squaredNorm();

		if (d2 > 0) {
			double df = 1e12 * d2;
			double rs = 1 / sqrt(df);
			e = save + rs * dx;
			if (df > 0) {
				lsum += log(df);
				NL++;
			}
			L = lsum / NL;
		}
		current = save;

	}

	void getCoeff();

	void storeCoeff();
	void storePoints();

	std::vector<Vector2d> xy;
	std::vector<double> coeffx, coeffy;
	VectorXd pows;
	Vector2d initial;
	Vector2d current;
	Vector2d last;
	Vector2d e, save;
	double lsum;
	int n_coeff;
	int O;
	int N;
	int NL;
	double L;
};

