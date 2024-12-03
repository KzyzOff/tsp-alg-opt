#pragma once

#include <random>

#include "types.hpp"
#include "Population.hpp"

using namespace tsp_t;

struct FlaggedIndividual {
	Individual i;
	bool is_elite;
};

class Selector {
public:
	Selector(Population& population, const Settings& settings, std::mt19937& rand_gen)
		: population(population), settings(settings), rand_gen(rand_gen) {}

	virtual std::vector<FlaggedIndividual> select_n(unsigned int) = 0;

protected:
	Population& population;
	const Settings& settings;
	std::mt19937& rand_gen;

};
