#pragma once

#include <random>
#include <map>

#include "Loader.hpp"
#include "types.hpp"

using namespace tsp_t;

// TODO: Add method for updating elite and method for returning just a pointer to current elite instead of recalculating it every get_n_best call
class Population {
public:
	Population(unsigned int pop_size, InitType init_type, Loader &loader, std::mt19937 &rand_gen);

	float calc_fitness(const std::vector<int> &chromosome);
	void swap_individuals(Individual &offspring, const Individual &parent);

	uint64_t get_fitness_update_count() const { return fitness_update_count; }
	std::multimap<float, std::vector<int> > &get_population() { return population; }
	std::multimap<float, std::vector<int> > get_n_best(int n) const;

	bool same_individuals(const Individual& i1, const Individual& i2) const;

private:
	bool is_initialized;
	std::mt19937 &rand_gen;
	Loader &loader;
	const LocationsPtr locations;
	std::multimap<float, std::vector<int> > population;

	uint64_t fitness_update_count;

	void random_init(unsigned int pop_size);

	// void greedy_init(unsigned int pop_size);
};
