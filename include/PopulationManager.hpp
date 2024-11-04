#pragma once

#include <random>

#include "types.hpp"
#include "Population.hpp"
#include "selectors/Selector.hpp"
#include "cross_operators/CrossOperator.hpp"

using namespace tsp_t;

// TODO: This class should manage flow of the algorithm
// TODO: Extract update and stats calculations into Population class
class PopulationManager {
public:
	PopulationManager(const Settings &settings, const LocationsPtr &locations);

	void update_pop_fitness();

	void advance_population();

	void mutate_population(MutationType mt);

	FitnessStats calc_fitness_stats();

	std::shared_ptr<const Individual> get_goat() const { return goat_individual; }

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

	std::shared_ptr<Individual> goat_individual;

	void mutate_inverse(Individual &individual);

	void mutate_swap(Individual &individual);
};
