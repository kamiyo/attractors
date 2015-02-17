#pragma once

#include "Generator.h"

#pragma once
class Generator2D : public Generator{
public:

	Generator2D(double initx, double inity, double initz, int order = 0)
		: initial(Vector3d(initx, inity, initz))
	{
		O = order;
		D = 3;
		reset();
	}

	Generator2D(double initx, double inity, int order = 0)
		: initial(Vector2d(initx, inity))
	{
		O = order;
		D = 2;
		reset();
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
		VectorXd next = step();
		VectorXd dx = next - save;
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

	void genCoeff();
	void getCoeff(std::ofstream& o) {
	/*	for (double v : coeffx) {
			o.write((char*) &v, sizeof(double));
		}
		for (double v : coeffy) {
			o.write((char*) &v, sizeof(double));
		}*/
	}

	void storeCoeff();
	void storePoints();

	MatrixXd points;
	MatrixXd coeff;
	MatrixXd pows;
	MatrixXi permutation;
	VectorXd initial;
	VectorXd current;
	VectorXd last;
	VectorXd e, save;
	double lsum;
	int N;
	int NL;
	double L;
};

