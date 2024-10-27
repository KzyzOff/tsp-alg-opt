#include "PopulationManager.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <random>
#include <utility>
#include <set>
#include <unordered_set>
#include <functional>
#include <list>

#include "utils.hpp"

PopulationManager::PopulationManager(unsigned int pop_size, float cross_prob, float mut_prob,
                                     LocationsPtr locations)
	: cross_pop_count(pop_size * cross_prob),
	  population_size(pop_size),
	  cross_prob(cross_prob),
	  mut_prob(mut_prob),
	  locations(locations),
	  chromosome_size(locations->size()),
	  rand_gen(rand_dev()),
	  population(pop_size, InitType::RANDOM, locations, rand_gen),
	  tour_selector(population.get_population(), rand_gen),
	  roulette_selector(population.get_population(), rand_gen),
	  ox_operator(population.get_population(), rand_gen, cross_pop_count),
	  pmx_operator(population.get_population(), rand_gen, cross_pop_count) {
	update_pop_fitness();
	goat_individual = *std::ranges::min_element(population.get_population(), [](const IndividualPtr &i1, const IndividualPtr &i2) {
		return i1->fitness < i2->fitness;
	});
}

// TODO: add precalculated lookup table for distances
void PopulationManager::update_pop_fitness() {
	for ( auto &individual : population.get_population() ) {
		update_fitness(*individual);
	}
}

void PopulationManager::update_fitness(Individual &individual) {
	individual.fitness = 0.f;
	for ( size_t i = 0; i < chromosome_size; ++i ) {
		if ( i == chromosome_size - 1 ) {
			individual.fitness += calc_distance(locations->at(individual.chromosome.at(i) - 1).x,
			                                    locations->at(individual.chromosome.at(i) - 1).y,
			                                    locations->at(individual.chromosome.at(0) - 1).x,
			                                    locations->at(individual.chromosome.at(0) - 1).y);
		} else {
			individual.fitness += calc_distance(locations->at(individual.chromosome.at(i) - 1).x,
			                                    locations->at(individual.chromosome.at(i) - 1).y,
			                                    locations->at(individual.chromosome.at(i + 1) - 1).x,
			                                    locations->at(individual.chromosome.at(i + 1) - 1).y);
		}
	}
}

FitnessStats PopulationManager::calc_fitness_stats() {
	auto comparator = [](const std::shared_ptr<Individual> &i1, const std::shared_ptr<Individual> &i2) {
		return i1->fitness < i2->fitness;
	};

	auto accumulator = [](float acc, const std::shared_ptr<Individual> &i) {
		return acc + i->fitness;
	};

	FitnessStats result {
		.worst = std::max_element(population.get_population().begin(), population.get_population().end(), comparator)->get()->fitness,
		.best = std::min_element(population.get_population().begin(), population.get_population().end(), comparator)->get()->fitness,
		.mean = std::accumulate(population.get_population().begin(), population.get_population().end(), 1.f, accumulator) / static_cast<float>(
			        population.get_population().size())
	};

	return result;
}

void PopulationManager::advance_population() {
	auto selected_parents = roulette_selector.select_n(cross_pop_count);
	std::vector<std::shared_ptr<Individual> > children;
	children.reserve(selected_parents.size());
	// Skips the last one if selected parents count from the population is odd
	for ( int i = 1; i < selected_parents.size(); i += 2 ) {
		auto crossover_result = pmx_operator.cross(*selected_parents.at(i - 1), *selected_parents.at(i));
		children.push_back(std::make_shared<Individual>(crossover_result.first));
		children.push_back(std::make_shared<Individual>(crossover_result.second));
	}

	add_reduce(children);
	mutate_population(MutationType::SWAP);
	update_pop_fitness();

	auto generation_best = *std::ranges::min_element(population.get_population(),
		[](const std::shared_ptr<Individual> &i1, const std::shared_ptr<Individual> &i2) {
				return i1->fitness < i2->fitness;
	});
	if ( generation_best->fitness < goat_individual->fitness )
		goat_individual = generation_best;
}

void PopulationManager::mutate_population(MutationType mt) {
	switch ( mt ) {
		case MutationType::INVERSE: {
			std::set<std::shared_ptr<Individual> > selected_individuals;
			int count = population_size * mut_prob;
			std::uniform_int_distribution<> distribution(0, population_size - 1);

			while ( selected_individuals.size() < count ) {
				int index = distribution(rand_gen);
				selected_individuals.insert(population.get_population().at(index));
			}

			for ( auto &i : selected_individuals ) {
				mutate_inverse(*i);
			}

			break;
		}

		case MutationType::SWAP: {
			std::vector<int> idx;
			idx.resize(population_size);
			for ( int i = 0; i < population_size; ++i ) {
				idx.at(i) = i;
			}

			std::ranges::shuffle(idx, rand_gen);
			for ( int i = 0; i < mut_prob * population_size; ++i ) {
				mutate_swap(*population.get_population().at(i));
			}

			break;
		}
	}
}

void PopulationManager::mutate_inverse(Individual &individual) {
	std::uniform_int_distribution<> distribution(0, chromosome_size - 1);
	int start = distribution(rand_gen);
	int end = distribution(rand_gen);
	while ( start == end )
		end = distribution(rand_gen);
	if ( start > end )
		std::swap(start, end);

	const int delta = end - start;
	const int mid = delta / 2;
	for ( int i = 0; i < mid; ++i ) {
		std::swap(individual.chromosome.at(start + i), individual.chromosome.at(end - i));
	}
}

void PopulationManager::mutate_swap(Individual &individual) {
	std::uniform_int_distribution<> distribution(0, chromosome_size - 1);
	int first = distribution(rand_gen);
	int second = distribution(rand_gen);
	while ( first == second )
		second = distribution(rand_gen);

	std::swap(individual.chromosome.at(first), individual.chromosome.at(second));
}

void PopulationManager::add_reduce(std::vector<std::shared_ptr<Individual>>& new_offspring) {
	// Add offspring to population
	for ( auto& individual : new_offspring ) {
		population.get_population().push_back(individual);
	}

	auto worst_individuals = map_n_worst_to_pop_vector_index(population.get_population().size() - population_size);

	// Reduce population
	for ( auto& [fitness, indices] : worst_individuals ) {
		for ( auto& i : indices ) {
			population.get_population().erase(population.get_population().begin() + i);
		}
	}
}

nWorstMap PopulationManager::map_n_worst_to_pop_vector_index(const unsigned int n) {
	nWorstMap tmp_map;
	// Map entire population and automatically sort it
	for ( int i = 0; i < population.get_population().size(); ++i ) {
		tmp_map[population.get_population().at(i)->fitness].push_back(i);
	}

	nWorstMap result;
	bool done = false;
	for ( int i = 0; auto& worst : tmp_map ) {
		if ( done ) break;

		for ( auto& worst_individual : worst.second ) {
			result[worst.first].push_back(worst_individual);
			if ( ++i >= n ) {
				done = true;
				break;
			}
		}
	}

	return result;
}
