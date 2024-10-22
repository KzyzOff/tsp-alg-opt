#pragma once

#include <random>

#include "Loader.hpp"

struct Individual {
	std::vector<int> chromosome;
	float fitness;
};

struct FitnessStats {
	float worst;
	float best;
	float mean;
};

enum class MutationType {
	INVERSE,
	SWAP
};

class PopulationManager {
public:
	PopulationManager(unsigned int pop_size, unsigned int gen_count, float cross_prob, float mut_prob,
	                  std::shared_ptr<const std::vector<Location>> locations);

	void generate_random_pop();
	void update_pop_fitness();
	void update_fitness(Individual& individual);

	void advance_population();
	std::vector<std::shared_ptr<Individual>> tournament_selector();
	std::pair<Individual, Individual> ordered_crossover(const Individual& parent1, const Individual& parent2);
	// Add new offspring to the population and then remove the worst ones
	void add_reduce(std::vector<std::shared_ptr<Individual>>);
	// Replace worst individuals from the previous generation
	void replace_worst(std::vector<std::shared_ptr<Individual>>);
	void mutate_population(MutationType mt);

	FitnessStats calc_fitness_stats();

private:
	unsigned int population_size;
	unsigned int generation_count;
	float cross_prob;
	float mut_prob;
	std::shared_ptr<const std::vector<Location>> locations;
	std::vector<std::shared_ptr<Individual>> population;
	size_t chromosome_size;

	std::random_device rand_dev;
	std::mt19937 rand_gen;

	void mutate_inverse(Individual& individual);
	void mutate_swap(Individual& individual);

};
