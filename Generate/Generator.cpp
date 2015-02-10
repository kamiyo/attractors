#include "Generator.h"

const double EPSILON = 1e-6;
const double MIN_COEFF = -5.0, MAX_COEFF = 5.0;

bool operator< (const std::pair<double, double>& lh, const std::pair<double, double>& rh) {
	return (lh.first < rh.first);
}

std::ostream& operator<< (std::ostream& o, const std::vector<std::pair<double, double> >& p) {
	for (auto i : p) {
		o << i.first << " " << i.second << std::endl;
	}
	return o;
}
