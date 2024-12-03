#pragma once

#include <iostream>
#include <random>

#include "Population.hpp"
#include "types.hpp"

using namespace tsp_t;

class CrossOperator {
public:
	CrossOperator(Population &population, std::mt19937 &rand_gen,
	              const unsigned int chosen_parent_count)
		: population(population), rand_gen(rand_gen),
		  pop_size(population.get_population().size()),
		  chromosome_size(
		                  population.get_population().begin()->second.
		                             size()
		                 ),
		  chosen_parent_count(chosen_parent_count) {}

	virtual std::pair<Individual, Individual> cross(const Individual &parent1, const Individual &parent2) = 0;

protected:
	Population &population;
	std::mt19937 &rand_gen;
	const int pop_size;
	const int chromosome_size;
	const unsigned int chosen_parent_count;

};
