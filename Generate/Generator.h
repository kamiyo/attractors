#pragma once

#include <array>
#include <queue>
#include <vector>
#include <Eigen/core>

typedef Eigen::VectorXd VectorXd;
typedef Eigen::ArrayXd ArrayXd;

class Generator {
public:
	const static int MAX_ITER = (int)5e5;
	const static int MIN_ITER = (int)5e2;
	const static int MAX_ORDER = 10;	// O+1 coeff
	const static double EPSILON;
	const static double MIN_COEFF, MAX_COEFF;

	virtual void reset() = 0;
	virtual void search() = 0;
	virtual int iterate() = 0;
	virtual void lyapunov() = 0;
	virtual void getCoeff() = 0;
	virtual void plot(int prev) = 0;

};

bool operator< (const std::pair<double, double>& lh, const std::pair<double, double>& rh);
std::ostream& operator<< (std::ostream& o, const std::vector<std::pair<double, double> >& p);