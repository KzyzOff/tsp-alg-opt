#include "Population.hpp"

#include <algorithm>
#include <random>

#include "utils.hpp"
#include "Greedy.hpp"

Population::Population(unsigned int pop_size, InitType init_type, Loader &loader, std::mt19937 &rand_gen)
	: is_initialized(false),
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
		if ( it->second == parent.second ) {
			population.erase(it);
			break;
		}
	}
	population.insert(offspring);
}

bool Population::same_individuals(const Individual &i1, const Individual &i2) const {
	if ( i1.first == i2.first && i1.second == i2.second ) {
		return true;
	}

	return false;
}

std::multimap<float, std::vector<int> > Population::get_n_best(const int n) const {
	if ( n > population.size() )
		return {};

	std::multimap<float, std::vector<int> > result;
	int i = 0;
	for ( const auto &[fitness, chromosome] : population ) {
		result.insert({fitness, chromosome});
		if ( i == n - 1 )
			break;
		++i;
	}

	return result;
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

		population.insert({fitness, chromosome});
	}
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
