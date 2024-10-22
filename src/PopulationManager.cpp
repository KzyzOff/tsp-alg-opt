#include "PopulationManager.hpp"

#include <algorithm>
#include <iostream>
#include <random>
#include <utility>
#include <set>

#include "utils.hpp"

PopulationManager::PopulationManager(unsigned int pop_size, unsigned int gen_count, float cross_prob, float mut_prob,
                                     std::shared_ptr<const std::vector<Location>> locations)
	: population_size(pop_size),
	  generation_count(gen_count),
	  cross_prob(cross_prob),
	  mut_prob(mut_prob),
	  locations(locations),
	  chromosome_size(locations->size()),
	  rand_gen(rand_dev()) {
	generate_random_pop();
	update_pop_fitness();
}

// TODO: add precalculated lookup table for distances
void PopulationManager::update_pop_fitness() {
	for ( auto &individual : population ) {
		update_fitness(*individual);
	}
}

void PopulationManager::update_fitness(Individual& individual) {
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
	auto comparator = [](const std::shared_ptr<Individual> i1, const std::shared_ptr<Individual> i2) {
		return i1->fitness < i2->fitness;
	};

	auto accumulator = [](float acc, const std::shared_ptr<Individual> i) {
		return acc + i->fitness;
	};

	FitnessStats result {
		.worst = std::min_element(population.begin(), population.end(), comparator)->get()->fitness,
		.best = std::max_element(population.begin(), population.end(), comparator)->get()->fitness,
		.mean = std::accumulate(population.begin(), population.end(), 1.f, accumulator) / static_cast<float>(
			        population.size())
	};

	return result;
}

void PopulationManager::generate_random_pop() {
	population.clear();
	population.resize(population_size);
	for ( size_t i = 0; i < population_size; ++i ) {
		population.at(i) = std::make_shared<Individual>();

		population.at(i)->chromosome.resize(locations->size());
		for ( int x = 0; x < locations->size(); ++x ) {
			population.at(i)->chromosome.at(x) = locations->at(x).n;
		}
		std::shuffle(population.at(i)->chromosome.begin(), population.at(i)->chromosome.end(), rand_gen);
	}
}

void PopulationManager::advance_population() {
	auto selected = tournament_selector();
	std::vector<std::shared_ptr<Individual>> children;
	children.reserve(selected.size() / 2);
	// Skips the last one if selected parents count from the population is odd
	for ( int i = 1; i < selected.size(); ++i ) {
		auto crossed_result = ordered_crossover(*(selected.at(i - 1)), *(selected.at(i)));
		children.push_back(std::make_shared<Individual>(crossed_result.first));
		children.push_back(std::make_shared<Individual>(crossed_result.second));
	}

	for ( auto& child : children ) {
		update_fitness(*child);
	}

	mutate_population(MutationType::SWAP);
}

std::vector<std::shared_ptr<Individual> >
PopulationManager::tournament_selector() {
	std::vector<std::shared_ptr<Individual> > result;
	std::vector<std::shared_ptr<Individual> > shuffle_lookup(population.begin(), population.end());
	std::ranges::shuffle(shuffle_lookup, rand_gen);
	int selected_count = cross_prob * static_cast<float>(population_size);
	result.reserve(selected_count);

	for ( int i = 0; i < selected_count; ++i ) {
		auto best = std::min_element(shuffle_lookup.begin(), shuffle_lookup.end(),
		                     [](const std::shared_ptr<Individual>& i1, const std::shared_ptr<Individual>& i2) {
			                     return i1->fitness < i2->fitness;
		                     });
		shuffle_lookup.erase(best);
		update_fitness(**best);
		result.push_back(std::move(std::make_shared<Individual>(**best)));
		std::ranges::shuffle(shuffle_lookup, rand_gen);
	}

	return result;
}

std::pair<Individual, Individual>
PopulationManager::ordered_crossover(const Individual& parent1, const Individual& parent2) {
	std::uniform_int_distribution<int> distribution(0, chromosome_size - 1);
	int cut_start = distribution(rand_gen);
	int cut_end = distribution(rand_gen);
	while ( cut_start == cut_end )
		cut_end = distribution(rand_gen);
	if ( cut_start > cut_end )
		std::swap(cut_start, cut_end);

	Individual child1 {};
	child1.chromosome.reserve(chromosome_size);
	child1.chromosome.insert(child1.chromosome.end(),
	                         parent1.chromosome.begin(), parent1.chromosome.begin() + cut_start);
	child1.chromosome.insert(child1.chromosome.end(), parent2.chromosome.begin() + cut_start,
	                         parent2.chromosome.begin() + cut_end);
	child1.chromosome.insert(child1.chromosome.end(), parent1.chromosome.begin() + cut_end,
	                         parent1.chromosome.end());
	update_fitness(child1);

	Individual child2 {};
	child2.chromosome.reserve(chromosome_size);
	child2.chromosome.insert(child2.chromosome.end(),
	                         parent2.chromosome.begin(), parent2.chromosome.begin() + cut_start);
	child2.chromosome.insert(child2.chromosome.end(), parent1.chromosome.begin() + cut_start,
	                         parent1.chromosome.begin() + cut_end);
	child2.chromosome.insert(child2.chromosome.end(), parent2.chromosome.begin() + cut_end,
	                         parent2.chromosome.end());

	update_fitness(child2);

	return {child1, child2};
}

void PopulationManager::mutate_population(MutationType mt) {
	switch ( mt ) {
		case MutationType::INVERSE: {
			std::set<std::shared_ptr<Individual>> selected_individuals;
			int count = population_size * mut_prob;
			std::uniform_int_distribution<> distribution(0, population_size - 1);

			while ( selected_individuals.size() < count ) {
				int index = distribution(rand_gen);
				selected_individuals.insert(population.at(index));
			}

			for ( auto& i : selected_individuals ) {
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
				mutate_swap(*population.at(i));
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
