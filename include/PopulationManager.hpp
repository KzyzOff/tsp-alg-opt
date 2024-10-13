#pragma once

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

class PopulationManager {
public:
	PopulationManager(unsigned int pop_size, unsigned int gen_count, float cross_prob, float mut_prob,
	                  std::shared_ptr<const std::vector<Location>> locations);

	void update_pop_fitness();

	FitnessStats calc_fitness_stats();

private:
	unsigned int population_size;
	unsigned int generation_count;
	float cross_prob;
	float mut_prob;
	std::shared_ptr<const std::vector<Location>> locations;
	std::vector<Individual> population;

	void generate_random_pop();
};
