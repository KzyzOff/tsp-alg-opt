#include "Greedy.hpp"

#include <utility>
#include <map>
#include <algorithm>

Greedy::Greedy(std::shared_ptr<const std::vector<Location> > locations, std::shared_ptr<Graph> lookup_graph)
	: locations(std::move(locations)),
	  lookup_graph(std::move(lookup_graph)) {
}

Solution Greedy::get_best_solution() {
	Solution best_solution;

	// Check every possible location as a starting point
	for ( int i = 0; i < locations->size(); ++i ) {
		Solution tmp_solution {};
		tmp_solution.path.reserve(locations->size());
		tmp_solution.path.push_back(i);

		std::map<float, std::vector<int>> tmp_lookup;

		// Check every combination of whats left
		for ( int j = 0; j < locations->size() - 1; ++j ) {
			tmp_lookup = get_lookup_map(*lookup_graph->get_location_distances(tmp_solution.path.back()), tmp_solution.path);

			tmp_solution.path.push_back(tmp_lookup.begin()->second.front());
			tmp_solution.fitness += tmp_lookup.begin()->first;
		}

		// Add distance from last location to the first in order to close the loop
		tmp_solution.fitness += lookup_graph->get_distance(tmp_solution.path.at(0), tmp_solution.path.at(tmp_solution.path.size() - 1));
		if ( best_solution.fitness == 0 || tmp_solution.fitness < best_solution.fitness )
			best_solution = tmp_solution;
	}

	return best_solution;
}

// Helper function: transform one "row" of the graph into map; 0-indexed
std::map<float, std::vector<int>> Greedy::get_lookup_map(const std::vector<float> &graph_row, const std::vector<int>& visited_locations) {
	std::map<float, std::vector<int>> result {};
	for ( int i = 0; i < graph_row.size(); ++i ) {
		if ( std::ranges::find(visited_locations, i) != visited_locations.end() )
			continue;

		result[graph_row.at(i)].push_back(i);
	}

	return result;
}
