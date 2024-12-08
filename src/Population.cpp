#include "Population.hpp"

#include <algorithm>
#include <cassert>
#include <random>

#include "utils.hpp"
#include "Greedy.hpp"

Population::Population(unsigned int pop_size, int elite_sz, InitType init_type, Loader &loader, std::mt19937 &rand_gen)
	: is_initialized(false),
	  elite_sz(elite_sz),
	  rand_gen(rand_gen),
	  loader(loader),
	  locations(loader.get_locations()),
	  fitness_update_count(0) {
	switch ( init_type ) {
		case InitType::RANDOM:
			random_init(pop_size);
			break;
		case InitType::GREEDY:
			// greedy_init(pop_size);
			break;
	}
	is_initialized = true;
}

float Population::calc_fitness(const std::vector<int> &chromosome) {
	float fitness = 0.f;
	const int chromosome_size = locations->size();
	for ( size_t i = 0; i < chromosome_size; ++i ) {
		if ( i == chromosome_size - 1 ) {
			fitness += calc_distance(locations->at(chromosome.at(i) - 1).x,
			                         locations->at(chromosome.at(i) - 1).y,
			                         locations->at(chromosome.at(0) - 1).x,
			                         locations->at(chromosome.at(0) - 1).y);
		} else {
			fitness += calc_distance(locations->at(chromosome.at(i) - 1).x,
			                         locations->at(chromosome.at(i) - 1).y,
			                         locations->at(chromosome.at(i + 1) - 1).x,
			                         locations->at(chromosome.at(i + 1) - 1).y);
		}
	}

	if ( is_initialized )
		++fitness_update_count;

	return fitness;
}

void Population::swap_individuals(Individual &offspring, const Individual &parent) {
	auto [it, end] = population.equal_range(parent.first);
	for ( ; it != population.end(); ++it ) {
		if ( it->second.chromosome == parent.second.chromosome ) {
			population.erase(it);
			break;
		}
	}

	population.insert(offspring);
}

bool Population::same_individuals(const Individual &i1, const Individual &i2) const {
	if ( i1.first == i2.first && i1.second.chromosome == i2.second.chromosome ) {
		return true;
	}

	return false;
}

void Population::update_elite() {
	int current_count = 0;
	for ( auto& individual : population ) {
		individual.second.is_elite = current_count < elite_sz;
		++current_count;
	}
}

void Population::random_init(unsigned int pop_size) {
	population.clear();
	for ( size_t i = 0; i < pop_size; ++i ) {
		std::vector<int> chromosome(locations->size());

		for ( int x = 0; x < locations->size(); ++x ) {
			chromosome.at(x) = locations->at(x).n;
		}
		std::ranges::shuffle(chromosome, rand_gen);
		float fitness = calc_fitness(chromosome);

		assert(("[random_init] Empty chromosome or 0.f fitness value!", !chromosome.size() != pop_size || fitness == 0.f));
		population.insert({fitness, {chromosome, false}});
	}

	update_elite();
}

/**
 * @brief Greedy init of the population; two-city random start
 */
// void Population::greedy_init(unsigned int pop_size) {
// 	population.clear();
// 	population.resize(pop_size);
// 	std::uniform_int_distribution<> distr(0, locations->size() - 1);
//
// 	for ( auto& individual : population ) {
// 		int first_idx = distr(rand_gen);
// 		int second_idx = distr(rand_gen);
// 		while ( first_idx == second_idx )
// 			second_idx = distr(rand_gen);
// 		individual = std::make_shared<Individual>();
// 		individual->chromosome.resize(locations->size());
// 		individual->chromosome.at(0) = locations->at(0).n;
// 		individual->chromosome.at(1) = locations->at(0).n;
//
// 		Greedy::calc_solution_with_initial_chromosome(*individual, loader, 2);
// 	}
// }
