#include "Loader.hpp"

#include <fstream>
#include <regex>

#include "utils.hpp"
#include "constants.hpp"

Loader::Loader(const std::filesystem::path &filename) {
	load(filename);
}

void Loader::load(const std::filesystem::path &filename) {
	std::ifstream file(DATASETS_PATH / filename);

	std::string line;
	std::smatch matches;
	std::regex pattern(R"(^\d+ \d+\.\d+ \d+\.\d+\s*$)", std::regex_constants::ECMAScript);

	while ( std::getline(file, line) ) {
		if ( std::regex_match(line, matches, pattern) ) {
			std::istringstream iss(matches[0]);
			Location loc {};
			iss >> loc.n >> loc.x >> loc.y;
			locations.push_back(loc);
		}
	}
}

Graph Loader::get_lookup_graph() {
	if ( ! lookup_graph ) {
		lookup_graph = Graph(locations.size());
		init_lookup_graph();
	}

	return lookup_graph.value();
}

void Loader::init_lookup_graph() {
	for ( int i = 0; i < locations.size(); ++i ) {
		for ( int j = i + 1; j < locations.size(); ++j ) {
			Location loc1 = locations.at(i);
			Location loc2 = locations.at(j);
			lookup_graph->add_edge(i, j, calc_distance(loc1.x, loc1.y, loc2.x, loc2.y));
		}
	}
}
