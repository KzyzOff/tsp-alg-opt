#pragma once

#include <random>

#include "types.hpp"

using namespace tsp_t;

// TODO: change the way individuals are stored -> std::map<[fitness], std::vector<Individual>> for a quick lookup on start and on end of this collection
class Population {
public:
	Population(unsigned int pop_size, InitType init_type, LocationsPtr locations, std::mt19937& rand_gen);

	IndividualPtrVec& get_population() { return population; }

private:
	std::mt19937& rand_gen;
	LocationsPtr locations;
	IndividualPtrVec population;

	IndividualPtr goat;

	void random_init(unsigned int pop_size);
	void greedy_init(unsigned int pop_size);

};
