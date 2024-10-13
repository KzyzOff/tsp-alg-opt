#include "Loader.hpp"

#include <fstream>
#include <regex>

void Loader::load(const char *filename) {
	const std::string path = std::string(DATASETS_PATH).append(filename);

	std::ifstream file(path);

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
