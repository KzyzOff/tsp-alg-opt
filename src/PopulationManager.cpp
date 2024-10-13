#include "PopulationManager.hpp"

#include <algorithm>
#include <random>
#include <utility>

#include "utils.hpp"

PopulationManager::PopulationManager(unsigned int pop_size, unsigned int gen_count, float cross_prob, float mut_prob,
                                     std::shared_ptr<const std::vector<Location>> locations)
	: population_size(pop_size),
	  generation_count(gen_count),
	  cross_prob(cross_prob),
	  mut_prob(mut_prob),
	  locations(std::move(locations)) {
	generate_random_pop();
	update_pop_fitness();
}

// TODO: add precalculated lookup table for distances
void PopulationManager::update_pop_fitness() {
	size_t chromosome_size = locations->size();
	for ( auto &individual : population ) {
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
}

FitnessStats PopulationManager::calc_fitness_stats() {
	auto comparator = [](const Individual &i1, const Individual &i2) {
		return i1.fitness < i2.fitness;
	};

	auto accumulator = [](float acc, const Individual& i) {
		return acc + i.fitness;
	};

	FitnessStats result {
		.worst = std::min_element(population.begin(), population.end(), comparator)->fitness,
		.best = std::max_element(population.begin(), population.end(), comparator)->fitness,
		.mean = std::accumulate(population.begin(), population.end(), 1.f, accumulator) / static_cast<float>(population.size())
	};

	return result;
}

void PopulationManager::generate_random_pop() {
	std::random_device rd;
	std::mt19937 generator(rd());
	population.resize(population_size);
	for ( size_t i = 0; i < population_size; ++i ) {
		population.at(i).chromosome.resize(locations->size());
		for ( int x = 0; x < locations->size(); ++x ) {
			population.at(i).chromosome.at(x) = locations->at(x).n;
		}
		std::shuffle(population.at(i).chromosome.begin(), population.at(i).chromosome.end(), generator);
	}
}
