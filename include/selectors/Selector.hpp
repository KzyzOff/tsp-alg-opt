#pragma once

#include <random>

#include "types.hpp"
#include "Population.hpp"

using namespace tsp_t;

class Selector {
public:
	Selector(Population& population, const Settings& settings, std::mt19937& rand_gen)
		: population(population), settings(settings), rand_gen(rand_gen) {}

	virtual std::vector<Individual> select_n(unsigned int) = 0;

protected:
	Population& population;
	const Settings& settings;
	std::mt19937& rand_gen;

};
