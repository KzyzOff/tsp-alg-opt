#include "Graph.hpp"

Graph::Graph(int n_vertices) {
	weights.resize(n_vertices, std::vector<float>(n_vertices, 0.f));
}

void Graph::add_edge(const int u, const int v, const float weight) {
	// Graph is 0-indexed and locations in input files are 1-indexed
	weights.at(u).at(v) = weight;
	weights.at(v).at(u) = weight;
}

float Graph::get_distance(const int u, const int v) const {
	return weights.at(u).at(v);
}

std::shared_ptr<std::vector<float>> Graph::get_location_distances(const int u) {
	return std::make_shared<std::vector<float>>(weights.at(u));
}
