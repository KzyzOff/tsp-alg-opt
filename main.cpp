#include "TSPSolver.hpp"

int main() {
	TSPSolver solver(100, 10000, .7f, .01f, "test1.tsp");
	solver.solve();

	return 0;
}
