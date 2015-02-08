#pragma once

#include <array>
#include <vector>

class Generator{
public:
	const static int MAX_ITER = (int)5e5;
	const static int MIN_ITER = (int)5e2;
	const static int MAX_ORDER = 10;	// O+1 coeff
	const static double EPSILON;
	const static double MIN_COEFF, MAX_COEFF;

	Generator(double init, int order = 0, double ratio = 0) 
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
		double df = 0;
		double x_pow_accum = 1;
		for (int i = 1; i < coeff.size(); i++) {
			df += i * coeff[i] * x_pow_accum;
			x_pow_accum *= current_x;
		}
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

	std::vector<double> x0, xn;
	std::vector<double> coeff;
	double first_x;
	double current_x;
	double r;
	double lsum;
	int O;
	int N;
	int NL;
	double L;
};