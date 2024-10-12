#pragma once

#include <vector>
#include <string>

#define DATASETS_PATH "./datasets/"

struct Location {
	int n;
	float x, y;
};

class Loader {
public:
	void load(const char* filename);

private:
	std::vector<Location> locations;

};
