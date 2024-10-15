#pragma once

#include <vector>
#include <memory>

class Graph {
public:
	explicit Graph(int size);

	void add_edge(int u, int v, float weight);
	float get_distance(int u, int v) const;
	std::shared_ptr<std::vector<float>> get_location_distances(int u);

private:
	std::vector<std::vector<float>> weights;
};
