#pragma once

#include <vector>
#include <memory>

#include "Graph.hpp"
#include "types.hpp"

using namespace tsp_t;

class Loader {
public:
	explicit Loader(const std::filesystem::path &filename);

	void load(const std::filesystem::path &filename);

	std::shared_ptr<Graph> get_lookup_graph();
	LocationsPtr get_locations() const {
		return std::make_shared<std::vector<Location>>(locations);
	}

private:
	std::vector<Location> locations;
	std::shared_ptr<Graph> lookup_graph = nullptr;

	void init_lookup_graph();

};
