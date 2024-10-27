#pragma once

#include <random>
#include <map>

#include "Loader.hpp"
#include "types.hpp"
#include "Population.hpp"
#include "selectors/RouletteSelector.hpp"
#include "selectors/TournamentSelector.hpp"
#include "cross_operators/OXOperator.hpp"
#include "cross_operators/PMXOperator.hpp"

enum class MutationType {
	INVERSE,
	SWAP
};

using nWorstMap = std::map<float, std::vector<int>, std::greater<>>;
using namespace tsp_t;

// TODO: This class should manage flow of the algorithm
// TODO: Extract update and stats calculations into Population class
class PopulationManager {
public:
	PopulationManager(unsigned int pop_size, float cross_prob, float mut_prob, LocationsPtr locations);

	void update_pop_fitness();
	void update_fitness(Individual& individual);

	void advance_population();

	void mutate_population(MutationType mt);

	// Add new offspring to the population and then remove the worst ones
	void add_reduce(std::vector<std::shared_ptr<Individual>>& new_offspring);

	FitnessStats calc_fitness_stats();
	std::shared_ptr<const Individual> get_goat() const { return goat_individual; }

private:
	float cross_prob;
	float mut_prob;
	const unsigned int cross_pop_count;
	unsigned int population_size;

	LocationsPtr locations;
	size_t chromosome_size;

	std::random_device rand_dev;
	std::mt19937 rand_gen;

	Population population;
	TournamentSelector tour_selector;
	RouletteSelector roulette_selector;
	OXOPerator ox_operator;
	PMXOperator pmx_operator;

	std::shared_ptr<Individual> goat_individual;

	void mutate_inverse(Individual &individual);
	void mutate_swap(Individual &individual);

	nWorstMap map_n_worst_to_pop_vector_index(unsigned int n);
};
