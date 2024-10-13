#include "Loader.hpp"
#include "PopulationManager.hpp"

int main() {
	Loader l;
	l.load("berlin52.tsp");

	PopulationManager pm(100, 100, .7f, .1f, l.get_locations());
	pm.calc_fitness_stats();

	return 0;
}
