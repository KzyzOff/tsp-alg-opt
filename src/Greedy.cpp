#include "Greedy.hpp"

#include <utility>
#include <map>
#include <algorithm>

Greedy::Greedy(std::shared_ptr<const std::vector<Location>> locations, std::shared_ptr<Graph> lookup_graph)
	: locations(std::move(locations)),
	  lookup_graph(std::move(lookup_graph)) {
}

void Greedy::calc_solution_with_initial_chromosome(Individual &initial_individual, Loader &data_loader,
                                                   const unsigned int start_idx) {
	initial_individual.fitness += data_loader.get_lookup_graph().get_distance(initial_individual.chromosome.at(0) - 1,
	                                                                          initial_individual.chromosome.at(1) - 1);
	// Check every combination of whats left
	for ( int j = (int) start_idx; j < data_loader.get_locations()->size(); ++j ) {
		auto tmp_lookup = get_lookup_map(*data_loader.get_lookup_graph().
		                                              get_location_distances(*(initial_individual.
		                                                                       chromosome.begin() + j - 1) - 1),
		                                 std::set(initial_individual.chromosome.begin(),
		                                          initial_individual.chromosome.begin() + j - 1)
		                                );

		auto second_el = ++tmp_lookup.begin();
		initial_individual.chromosome.at(j) = second_el->second.front() + 1;
		initial_individual.fitness += second_el->first;
	}

	// Add distance from last location to the first in order to close the loop
	initial_individual.fitness += data_loader.get_lookup_graph().get_distance(initial_individual.chromosome.at(0),
	                                                                          initial_individual.chromosome.
	                                                                          at(initial_individual.chromosome.size() - 1));
}

Individual Greedy::get_best_solution() {
	Individual best_solution;

	// Check every possible location as a starting point
	for ( int i = 0; i < locations->size(); ++i ) {
		Individual tmp_solution {};
		tmp_solution.chromosome.reserve(locations->size());
		tmp_solution.chromosome.push_back(i);

		std::map<float, std::vector<int> > tmp_lookup;

		// Check every combination of whats left
		for ( int j = 0; j < locations->size() - 1; ++j ) {
			tmp_lookup = get_lookup_map(*lookup_graph->get_location_distances(tmp_solution.chromosome.back()),
			                            std::set(tmp_solution.chromosome.begin(), tmp_solution.chromosome.end()));

			tmp_solution.chromosome.push_back(tmp_lookup.begin()->second.front());
			tmp_solution.fitness += tmp_lookup.begin()->first;
		}

		// Add distance from last location to the first in order to close the loop
		tmp_solution.fitness += lookup_graph->get_distance(tmp_solution.chromosome.at(0),
		                                                   tmp_solution.chromosome.
		                                                                at(tmp_solution.chromosome.size() - 1));
		if ( best_solution.fitness == 0 || tmp_solution.fitness < best_solution.fitness )
			best_solution = tmp_solution;
	}

	return best_solution;
}

// Helper function: transform one "row" of the graph into map; 0-indexed
std::map<float, std::vector<int> > Greedy::get_lookup_map(const std::vector<float> &graph_row,
                                                          const std::set<int> &visited_locations) {
	std::map<float, std::vector<int> > result {};
	for ( int i = 0; i < graph_row.size(); ++i ) {
		if ( std::ranges::find(visited_locations, i + 1) != visited_locations.end() )
			continue;

		result[graph_row.at(i)].push_back(i);
	}

	return result;
}
