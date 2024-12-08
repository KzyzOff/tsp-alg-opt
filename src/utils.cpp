#include "utils.hpp"

#include <cmath>
#include <sstream>

#include "TSPSolver.hpp"

float calc_distance(float x1, float y1, float x2, float y2) {
	float dx = std::abs(x1 - x2);
	float dy = std::abs(y1 - y2);

	return std::sqrt(dx * dx + dy * dy);
}

std::string vec2str(const std::vector<int>& vec) {
	std::ostringstream result;
	for ( auto el : vec ) {
		result << el;
	}

	return result.str();
}

void mt_run(const Settings settings, int id) {
	TSPSolver s(settings);
	s.solve(std::pow(10, 6), id);
	printf("Simulation %i has been completed.\n", id);
}
