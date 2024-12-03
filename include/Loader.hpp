#pragma once

#include <vector>
#include <memory>
#include <optional>

#include "Graph.hpp"
#include "types.hpp"

using namespace tsp_t;

class Loader {
public:
	explicit Loader(const std::filesystem::path &filename);

	void load(const std::filesystem::path &filename);

	Graph get_lookup_graph();
	LocationsPtr get_locations() const {
		return std::make_shared<std::vector<Location>>(locations);
	}

private:
	std::vector<Location> locations;
	std::optional<Graph> lookup_graph = std::nullopt;

	void init_lookup_graph();

};
