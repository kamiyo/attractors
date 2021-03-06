#pragma once

#include "Generator.h"

#pragma once
class Generator1D : public Generator{
public:

	Generator1D(double init, int order = 0, double ratio = 0)
		: first_x(init)
		, r(ratio)
	{
		O = order;
		D = 1;
		reset();
	}

	void reset();

	void search();
	int iterate();
	Vector2d step();
	

	void genCoeff();
	void getCoeff(std::ofstream& o) {
		for (double v : coeff) {
			o.write((char*) &v, sizeof(double));
		}
	}

	void storeCoeff();
	void storePoints();

	std::vector<double> xs, ys;
	std::queue<double> buff;
	std::vector<double> coeff;
	VectorXd xpow, dx;
	double first_x;
	double prev_x;
	double current_x;
	double r;
	double lsum;
	int N;
	int NL;
	double L;
};
