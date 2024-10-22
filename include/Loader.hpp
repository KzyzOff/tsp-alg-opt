#pragma once

#include <vector>
#include <memory>

#include "Graph.hpp"

struct Location {
	int n;
	float x, y;
};

class Loader {
public:
	explicit Loader(const std::string& filename);

	void load(const char* filename);

	std::shared_ptr<Graph> get_lookup_graph();
	std::shared_ptr<const std::vector<Location>> get_locations() const {
		return std::make_shared<std::vector<Location>>(locations);
	}

private:
	std::vector<Location> locations;
	std::shared_ptr<Graph> lookup_graph = nullptr;

	void init_lookup_graph();

};
