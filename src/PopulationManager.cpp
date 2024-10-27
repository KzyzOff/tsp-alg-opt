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

#include "constants.hpp"
#include "utils.hpp"

PopulationManager::PopulationManager(unsigned int pop_size, unsigned int gen_count, float cross_prob, float mut_prob,
                                     std::shared_ptr<const std::vector<Location>> locations)
	: population_size(pop_size),
	  generation_count(gen_count),
	  cross_prob(cross_prob),
	  mut_prob(mut_prob),
	  locations(locations),
	  chromosome_size(locations->size()),
	  rand_gen(rand_dev()),
	  tour_selector(population, rand_gen),
	  roulette_selector(population, rand_gen),
	  cross_pop_count(pop_size * cross_prob) {
	generate_random_pop();
	update_pop_fitness();
}

// TODO: add precalculated lookup table for distances
void PopulationManager::update_pop_fitness() {
	for ( auto &individual : population ) {
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
		.worst = std::max_element(population.begin(), population.end(), comparator)->get()->fitness,
		.best = std::min_element(population.begin(), population.end(), comparator)->get()->fitness,
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
		update_fitness(*population.at(i));
	}

	goat_individual = *std::ranges::min_element(population, [](const IndividualPtr& i1, const IndividualPtr& i2) {
		return i1->fitness < i2->fitness;
	});
}

void PopulationManager::advance_population() {
	auto selected_parents = roulette_selector.select_n(cross_pop_count);
	std::vector<std::shared_ptr<Individual> > children;
	children.reserve(selected_parents.size());
	// Skips the last one if selected parents count from the population is odd
	for ( int i = 1; i < selected_parents.size(); i += 2 ) {
		auto crossover_result = ox_crossover(*selected_parents.at(i - 1), *selected_parents.at(i));
		children.push_back(std::make_shared<Individual>(crossover_result.first));
		children.push_back(std::make_shared<Individual>(crossover_result.second));
	}

	add_reduce(children);
	mutate_population(MutationType::SWAP);
	update_pop_fitness();

	auto generation_best = *std::ranges::min_element(population,
		[](const std::shared_ptr<Individual> &i1, const std::shared_ptr<Individual> &i2) {
				return i1->fitness < i2->fitness;
	});
	if ( generation_best->fitness < goat_individual->fitness )
		goat_individual = generation_best;
}

std::pair<Individual, Individual>
PopulationManager::ox_crossover(const Individual &parent1, const Individual &parent2) {
	std::uniform_int_distribution<int> distribution(0, chromosome_size - 1);
	int cut_start = distribution(rand_gen);
	int cut_end = distribution(rand_gen);
	while ( cut_start == cut_end )
		cut_end = distribution(rand_gen);
	if ( cut_start > cut_end )
		std::swap(cut_start, cut_end);

	auto offspring1 = cross_parents_ox(parent1, parent2, cut_start, cut_end);
	auto offspring2 = cross_parents_ox(parent2, parent1, cut_start, cut_end);

	return {offspring1, offspring2};
}

Individual PopulationManager::cross_parents_ox(const Individual &parent1, const Individual &parent2,
                                                 const int cut_start, const int cut_end) {
	const std::set<int> used_genes(parent1.chromosome.begin() + cut_start, parent1.chromosome.begin() + cut_end);
	std::vector<int> remaining_genes;
	remaining_genes.resize(chromosome_size - (cut_end - cut_start));
	int remaining_idx = 0;
	for ( int i = 0; i < chromosome_size; ++i ) {
		if ( used_genes.contains(parent2.chromosome.at(i)) ) continue;

		remaining_genes.at(remaining_idx) = parent2.chromosome.at(i);
		++remaining_idx;
	}

	Individual offspring;
	offspring.chromosome.reserve(chromosome_size);

	for ( int i = 0; i < remaining_genes.size(); ++i ) {
		if ( i == cut_start ) {
			offspring.chromosome.insert(offspring.chromosome.end(), parent1.chromosome.begin() + cut_start,
										parent1.chromosome.begin() + cut_end);
		}

		offspring.chromosome.push_back(remaining_genes.at(i));
	}

	return offspring;
}

// BUG: program crashes sometimes because of invalid map.at() operation
std::pair<Individual, Individual>
PopulationManager::pmx_crossover(const Individual &parent1, const Individual &parent2) {
	std::uniform_int_distribution<> distribution(0, chromosome_size - 1);
	int cut_start = distribution(rand_gen);
	int cut_end = distribution(rand_gen);
	while ( cut_start == cut_end )
		cut_end = distribution(rand_gen);
	if ( cut_start > cut_end )
		std::swap(cut_start, cut_end);

	std::pair<Individual, Individual> offspring {};
	offspring.first.chromosome.resize(chromosome_size);
	offspring.second.chromosome.resize(chromosome_size);
	std::ranges::fill(offspring.first.chromosome, -1);
	std::ranges::fill(offspring.second.chromosome, -1);
	for ( int i = cut_start; i <= cut_end; ++i ) {
		offspring.first.chromosome.at(i) = parent1.chromosome.at(i);
		offspring.second.chromosome.at(i) = parent2.chromosome.at(i);
	}

	map_remaining_pmx(parent1, parent2, offspring.first, cut_start, cut_end);
	map_remaining_pmx(parent2, parent1, offspring.second, cut_start, cut_end);

	return offspring;
}

/**
 * 
 * @param parent1 parent, from which the cut-span chromosomes were taken
 * @param parent2 second parent
 * @param offspring result of the crossover
 * @param cut_start start point of the cut-span
 * @param cut_end end point of the cut-span
 */
void PopulationManager::map_remaining_pmx(const Individual &parent1, const Individual &parent2, Individual &offspring,
                                          const int cut_start, const int cut_end) {
	using std::ranges::find;
	// <parent1 gene, parent2 gene>
	std::map<int, int> parent1_to_parent2_map;
	std::set<int> used_genes;

	int cut_distance = cut_end - cut_start + 1;
	for ( int i = cut_start; i <= cut_distance; ++i ) {
		parent1_to_parent2_map.insert({parent1.chromosome.at(i), parent2.chromosome.at(i)});
		used_genes.insert(parent1.chromosome.at(i));
	}

	for ( int i = 0; i < chromosome_size; ++i ) {
		if ( offspring.chromosome.at(i) != -1 ) continue;

		int gene_to_insert = parent2.chromosome.at(i);
		while ( used_genes.contains(gene_to_insert) ) {
			gene_to_insert = parent1_to_parent2_map.at(gene_to_insert);
		}
		offspring.chromosome.at(i) = gene_to_insert;
		used_genes.insert(gene_to_insert);
	}
}

void PopulationManager::mutate_population(MutationType mt) {
	switch ( mt ) {
		case MutationType::INVERSE: {
			std::set<std::shared_ptr<Individual> > selected_individuals;
			int count = population_size * mut_prob;
			std::uniform_int_distribution<> distribution(0, population_size - 1);

			while ( selected_individuals.size() < count ) {
				int index = distribution(rand_gen);
				selected_individuals.insert(population.at(index));
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

void PopulationManager::add_reduce(std::vector<std::shared_ptr<Individual>>& new_offspring) {
	// Add offspring to population
	for ( auto& individual : new_offspring ) {
		population.push_back(individual);
	}

	auto worst_individuals = map_n_worst_to_pop_vector_index(population.size() - population_size);

	// Reduce population
	for ( auto& [fitness, indices] : worst_individuals ) {
		for ( auto& i : indices ) {
			population.erase(population.begin() + i);
		}
	}
}

nWorstMap PopulationManager::map_n_worst_to_pop_vector_index(const unsigned int n) {
	nWorstMap tmp_map;
	// Map entire population and automatically sort it
	for ( int i = 0; i < population.size(); ++i ) {
		tmp_map[population.at(i)->fitness].push_back(i);
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
