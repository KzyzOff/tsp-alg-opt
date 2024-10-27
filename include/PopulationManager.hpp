#pragma once

#include <random>
#include <map>

#include "Loader.hpp"
#include "types.hpp"
#include "selectors/RouletteSelector.hpp"
#include "selectors/TournamentSelector.hpp"

enum class MutationType {
	INVERSE,
	SWAP
};

using nWorstMap = std::map<float, std::vector<int>, std::greater<>>;
using namespace tsp_t;

class PopulationManager {
public:
	PopulationManager(unsigned int pop_size, unsigned int gen_count, float cross_prob, float mut_prob,
	                  std::shared_ptr<const std::vector<Location>> locations);

	void generate_random_pop();
	void update_pop_fitness();
	void update_fitness(Individual& individual);

	void advance_population();

	std::pair<Individual, Individual> ox_crossover(const Individual& parent1, const Individual& parent2);
	std::pair<Individual, Individual> pmx_crossover(const Individual& parent1, const Individual& parent2);

	void mutate_population(MutationType mt);

	// Add new offspring to the population and then remove the worst ones
	void add_reduce(std::vector<std::shared_ptr<Individual>>& new_offspring);

	FitnessStats calc_fitness_stats();
	std::shared_ptr<const Individual> get_goat() const { return goat_individual; }

private:
	TournamentSelector tour_selector;
	RouletteSelector roulette_selector;

	const unsigned int cross_pop_count;

	unsigned int population_size;
	unsigned int generation_count;
	float cross_prob;
	float mut_prob;
	std::shared_ptr<const std::vector<Location>> locations;
	IndividualPtrVec population;
	std::shared_ptr<Individual> goat_individual;
	size_t chromosome_size;

	std::random_device rand_dev;
	std::mt19937 rand_gen;

	void map_remaining_pmx(const Individual &parent1, const Individual &parent2, Individual &offspring,
	                       const int cut_start, const int cut_end);

	Individual cross_parents_ox(const Individual& parent1, const Individual& parent2,
								const int cut_start, const int cut_end);

	void mutate_inverse(Individual &individual);
	void mutate_swap(Individual &individual);

	nWorstMap map_n_worst_to_pop_vector_index(const unsigned int n);
};
