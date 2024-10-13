#pragma once

#include <vector>
#include <memory>

#define DATASETS_PATH "./datasets/"

struct Location {
	int n;
	float x, y;
};

class Loader {
public:
	void load(const char* filename);

	std::shared_ptr<const std::vector<Location>> get_locations() const {
		return std::make_shared<std::vector<Location> >(locations);
	}

private:
	std::vector<Location> locations;

};
