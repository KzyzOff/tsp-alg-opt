#include "utils.hpp"

#include <cmath>

float calc_distance(float x1, float y1, float x2, float y2) {
	float dx = std::abs(x1 - x2);
	float dy = std::abs(y1 - y2);

	return std::sqrt(dx * dx + dy * dy);
}
