#pragma once

#include <memory>
#include <vector>
#include <map>
#include <set>

#include "Loader.hpp"
#include "types.hpp"

class Greedy {
public:
	Greedy(std::shared_ptr<const std::vector<Location>> locations, const Graph& lookup_graph);

	Individual get_solution(int start_location_id);
	Individual get_best_solution();

	static void calc_solution_with_initial_chromosome(Individual &initial_individual, Loader &data_loader,
	                                                  unsigned int start_idx);

	static std::map<float, std::vector<int> > get_lookup_map(const std::vector<float> &graph_row,
	                                                         const std::set<int> &visited_locations);

private:
	std::shared_ptr<const std::vector<Location>> locations;
	Graph lookup_graph;
};
