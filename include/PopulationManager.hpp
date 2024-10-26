#pragma once

#include <random>
#include <map>
#include <unordered_set>

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

using nWorstMap = std::map<float, std::vector<int>, std::greater<> >;

class PopulationManager {
public:
	PopulationManager(unsigned int pop_size, unsigned int gen_count, float cross_prob, float mut_prob,
	                  std::shared_ptr<const std::vector<Location>> locations);

	void generate_random_pop();
	void update_pop_fitness();
	void update_fitness(Individual& individual);

	void advance_population();
	std::vector<std::shared_ptr<Individual>> tournament_selector();
	std::vector<std::shared_ptr<Individual>> roulette_selector();

	std::pair<Individual, Individual> ox_crossover(const Individual& parent1, const Individual& parent2);
	std::pair<Individual, Individual> pmx_crossover(const Individual& parent1, const Individual& parent2);

	void mutate_population(MutationType mt);

	// Add new offspring to the population and then remove the worst ones
	void add_reduce(std::vector<std::shared_ptr<Individual>>& new_offspring);

	FitnessStats calc_fitness_stats();
	std::shared_ptr<const Individual> get_goat() const { return goat_individual; }

private:
	unsigned int population_size;
	unsigned int generation_count;
	float cross_prob;
	float mut_prob;
	std::shared_ptr<const std::vector<Location>> locations;
	std::vector<std::shared_ptr<Individual> > population;
	std::shared_ptr<Individual> goat_individual;
	size_t chromosome_size;

	std::random_device rand_dev;
	std::mt19937 rand_gen;

	std::vector<std::shared_ptr<Individual>> get_n_best(const int n);

	void map_remaining_pmx(const Individual &parent1, const Individual &parent2, Individual &offspring,
	                       const int cut_start, const int cut_end);

	Individual cross_parents_ox(const Individual& parent1, const Individual& parent2,
								const int cut_start, const int cut_end);

	void mutate_inverse(Individual &individual);
	void mutate_swap(Individual &individual);

	nWorstMap map_n_worst_to_pop_vector_index(const unsigned int n);
};
