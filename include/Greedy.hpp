#pragma once

#include <memory>
#include <vector>
#include <map>

#include "Loader.hpp"

struct Solution {
	std::vector<int> path {};
	float fitness = 0.f;
};

class Greedy {
public:
	Greedy(std::shared_ptr<const std::vector<Location>> locations, std::shared_ptr<Graph> lookup_graph);

	Solution get_best_solution();

private:
	std::shared_ptr<const std::vector<Location>> locations;
	std::shared_ptr<Graph> lookup_graph;

	std::map<float, std::vector<int>> get_lookup_map(const std::vector<float>& graph_row, const std::vector<int>& visited_locations);

};
