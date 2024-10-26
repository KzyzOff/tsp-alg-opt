#pragma once

#include <random>

#include "types.hpp"

using namespace tsp_t;

class Selector {
public:
	Selector(IndividualPtrVec& population, std::mt19937& rand_gen)
		: population(population), rand_gen(rand_gen) {}

	virtual IndividualPtrVec select_n(unsigned int) = 0;

protected:
	IndividualPtrVec& population;
	std::mt19937& rand_gen;

};
