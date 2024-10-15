#include <iostream>

#include "Loader.hpp"
#include "PopulationManager.hpp"
#include "Graph.hpp"
#include "Greedy.hpp"

int main() {
	Loader l;
	l.load("berlin52.tsp");

	auto graph = l.get_lookup_graph();
	Greedy greedy(l.get_locations(), graph);
	auto res = greedy.get_best_solution();

	std::cout << "Best greedy algo solution is: " << res.fitness << std::endl;

	// PopulationManager pm(100, 100, .7f, .1f, l.get_locations());
	// pm.calc_fitness_stats();

	return 0;
}
