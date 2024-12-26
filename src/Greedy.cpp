#include "Greedy.hpp"

#include <utility>
#include <map>
#include <algorithm>

Greedy::Greedy(std::shared_ptr<const std::vector<Location>> locations, const Graph &lookup_graph)
	: locations(std::move(locations)),
	  lookup_graph(lookup_graph) {
}

void Greedy::calc_solution_with_initial_chromosome(Individual &initial_individual, Loader &data_loader,
                                                   const unsigned int start_idx) {
	initial_individual.first += data_loader.get_lookup_graph().get_distance(initial_individual.second.chromosome.at(0) -
	                                                                        1,
	                                                                        initial_individual.second.chromosome.at(1) -
	                                                                        1);
	// Check every combination of whats left
	for ( int j = (int) start_idx; j < data_loader.get_locations()->size(); ++j ) {
		auto tmp_lookup = get_lookup_map(*data_loader.get_lookup_graph().
		                                              get_location_distances(*(initial_individual.
		                                                                       second.chromosome.begin() + j - 1) - 1),
		                                 std::set(initial_individual.second.chromosome.begin(),
		                                          initial_individual.second.chromosome.begin() + j - 1)
		                                );

		auto second_el = ++tmp_lookup.begin();
		initial_individual.second.chromosome.at(j) = second_el->second.front() + 1;
		initial_individual.first += second_el->first;
	}

	// Add distance from last location to the first in order to close the loop
	initial_individual.first += data_loader.get_lookup_graph().get_distance(initial_individual.second.chromosome.at(0),
	                                                                        initial_individual.second.chromosome.
	                                                                        at(initial_individual.second.chromosome.
	                                                                           size() - 1));
}

Individual Greedy::get_solution(int start_location_id) {
	// IDs are 1-indexed, but everything else -> 0-indexed
	--start_location_id;

	float current_fitness = 0.f;
	std::vector<int> chromosome;
	chromosome.reserve(locations->size());
	chromosome.push_back(locations->at(start_location_id).n);

	std::set<int> visited_locs {*chromosome.begin()};

	for ( int i = 0; i < locations->size() - 1; ++i ) {
		auto tmp_lookup = get_lookup_map(
		                                 *lookup_graph.get_location_distances(chromosome.back() - 1),
		                                 visited_locs
		                                );

		auto chosen_loc = *tmp_lookup.begin();
		chromosome.push_back(chosen_loc.second.front());
		current_fitness += chosen_loc.first;
		visited_locs.insert(chosen_loc.second.front());
	}

	// Add distance from last location to the first in order to close the loop
	current_fitness += lookup_graph.get_distance(chromosome.front() - 1, chromosome.back() - 1);

	return {current_fitness, {chromosome, false}};
}

Individual Greedy::get_best_solution() {
	Individual best_solution;

	// Check every possible location as a starting point
	for ( int i = 0; i < locations->size(); ++i ) {
		Individual tmp_solution {};
		tmp_solution.second.chromosome.reserve(locations->size());
		tmp_solution.second.chromosome.push_back(i);

		std::map<float, std::vector<int> > tmp_lookup;

		// Check every combination of whats left
		for ( int j = 0; j < locations->size() - 1; ++j ) {
			tmp_lookup = get_lookup_map(*lookup_graph.get_location_distances(tmp_solution.second.chromosome.back()),
			                            std::set(tmp_solution.second.chromosome.begin(),
			                                     tmp_solution.second.chromosome.end()));

			tmp_solution.second.chromosome.push_back(tmp_lookup.begin()->second.front());
			tmp_solution.first += tmp_lookup.begin()->first;
		}

		// Add distance from last location to the first in order to close the loop
		tmp_solution.first += lookup_graph.get_distance(tmp_solution.second.chromosome.at(0),
		                                                tmp_solution.second.chromosome.
		                                                             at(tmp_solution.second.chromosome.size() - 1));
		if ( best_solution.first == 0 || tmp_solution.first < best_solution.first )
			best_solution = tmp_solution;
	}

	return best_solution;
}

// Helper function: transform one "row" of the graph into map; 0-indexed
std::map<float, std::vector<int> > Greedy::get_lookup_map(const std::vector<float> &graph_row,
                                                          const std::set<int> &visited_locations) {
	std::map<float, std::vector<int> > result {};
	for ( int i = 0; i < graph_row.size(); ++i ) {
		if ( graph_row.at(i) != 0 &&
		     std::ranges::find(visited_locations, i + 1) == visited_locations.end() ) {
			result[graph_row.at(i)].push_back(i + 1);
		}
	}

	return result;
}
