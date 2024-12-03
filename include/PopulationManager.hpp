#pragma once

#include <random>

#include "types.hpp"
#include "Loader.hpp"
#include "Population.hpp"
#include "selectors/Selector.hpp"
#include "cross_operators/CrossOperator.hpp"

using namespace tsp_t;

// TODO: Extract update and stats calculations into Population class
class PopulationManager {
public:
	PopulationManager(const Settings &settings, Loader &loader);

	void advance_population();
	void mutate_population();

	FitnessStats calc_fitness_stats();
	uint64_t get_fitness_update_count() const { return population.get_fitness_update_count(); }

private:
	const Settings& settings;

	const unsigned int cross_pop_count;
	const unsigned int mut_pop_count;
	size_t chromosome_size;

	std::random_device rand_dev;
	std::mt19937 rand_gen;

	Population population;
	std::unique_ptr<CrossOperator> cross_operator;
	std::unique_ptr<Selector> selector;
	MutationType mut_type;

	void mutate_inverse(std::vector<int>& chromosome);
	void mutate_swap(std::vector<int>& chromosome);
};
