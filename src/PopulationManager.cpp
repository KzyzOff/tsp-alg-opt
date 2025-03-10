#include "PopulationManager.hpp"

#include <algorithm>
#include <map>
#include <random>
#include <utility>
#include <set>
#include <unordered_set>
#include <functional>

#include "cross_operators/OXOperator.hpp"
#include "cross_operators/PMXOperator.hpp"
#include "selectors/TournamentSelector.hpp"
// #include "selectors/RouletteSelector.hpp"
#include <cassert>

#include "utils.hpp"

PopulationManager::PopulationManager(const Settings &settings, Loader &loader)
	: settings(settings),
	  cross_pop_count(settings.pop_size * settings.cross_prob),
	  mut_pop_count(settings.pop_size * settings.mut_prob),
	  chromosome_size(loader.get_locations()->size()),
	  rand_gen(rand_dev()),
	  population(settings.pop_size, settings.elite_sz, settings.init_t, loader, rand_gen),
	  mut_type() {
	switch ( settings.cross_t ) {
		case CrossoverType::OX:
			cross_operator = std::make_unique<OXOPerator>(population, rand_gen, cross_pop_count);
			break;
		case CrossoverType::PMX:
			cross_operator = std::make_unique<PMXOperator>(population, rand_gen, cross_pop_count);
			break;
		case CrossoverType::NONE:
			cross_operator = nullptr;
			break;
	}

	switch ( settings.sel_t ) {
		case SelectionType::TOURNAMENT:
			selector = std::make_unique<TournamentSelector>(population, settings, rand_gen);
			break;
		case SelectionType::ROULETTE:
			// selector = std::make_unique<RouletteSelector>(population, rand_gen);
			break;
	}

	goat = *population.get_population().begin();
}

FitnessStats PopulationManager::calc_fitness_stats() {
	float best = population.get_population().begin()->first;
	float worst = std::next(population.get_population().begin(), settings.pop_size - 1)->first;
	float mean = 0.f;

	for ( const auto& [fitness, chromosome] : population.get_population() ) {
		mean += fitness;
	}

	mean /= (float)settings.pop_size;

	return {worst, best, mean};
}

void PopulationManager::advance_population() {
	if ( selector != nullptr && cross_operator != nullptr ) {
		auto selected_parents = selector->select_n(cross_pop_count);
		std::vector<Individual> children(cross_pop_count);

		// Skips the last one if selected parents count from the population is odd
		for ( int i = 1; i < cross_pop_count; i += 2 ) {
			auto [offspring1, offspring2] = cross_operator->cross(selected_parents.at(i - 1), selected_parents.at(i));
			offspring1.first = population.calc_fitness(offspring1.second.chromosome);
			offspring2.first = population.calc_fitness(offspring1.second.chromosome);
			children.at(i - 1) = offspring1;
			children.at(i) = offspring1;

			assert(("[advance_population] Fitness value cannot be 0.f at this point!", selected_parents.at(i).first != 0.f));
			assert(("[advance_population] Fitness value cannot be 0.f at this point!", selected_parents.at(i - 1).first != 0.f));
		}

		int offspring_idx = 0;
		for ( auto& parent : selected_parents ) {
			if ( !parent.second.is_elite ) {
				population.swap_individuals(children.at(offspring_idx), parent);
				++offspring_idx;
			}
		}
	}

	mutate_population();
	population.update_elite();

	const auto [fitness, chromosome] = *population.get_population().begin();
	if ( fitness < goat.first )
		goat = { fitness, chromosome };
}

void PopulationManager::mutate_population() {
	switch ( settings.mut_t ) {
		case MutationType::NONE:
			break;

		case MutationType::INVERSE: {
			std::set<Individual> mutated_individuals;
			std::set<Individual> selected_individuals;
			std::uniform_int_distribution<> distribution(0, settings.pop_size - 1);

			while ( mutated_individuals.size() < mut_pop_count ) {
				auto rand_individual = std::next(population.get_population().begin(), distribution(rand_gen));
				mutate_inverse(rand_individual->second.chromosome);
				Individual mutated {
					population.calc_fitness(rand_individual->second.chromosome),
					rand_individual->second
				};
				population.swap_individuals(mutated, *rand_individual);
				mutated_individuals.insert(mutated);
			}

			break;
		}

		case MutationType::SWAP: {
			std::uniform_int_distribution<> distribution(0, settings.pop_size - 1);
			for ( int i = 0; i < mut_pop_count; ++i ) {
				auto rand_individual = std::next(population.get_population().begin(), distribution(rand_gen));
				mutate_swap(rand_individual->second.chromosome);
				Individual mutated {
					population.calc_fitness(rand_individual->second.chromosome),
					rand_individual->second
				};
				population.swap_individuals(mutated, *rand_individual);
			}

			break;
		}
	}
}

void PopulationManager::mutate_inverse(std::vector<int>& chromosome) {
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
		std::swap(chromosome.at(start + i), chromosome.at(end - i));
	}
}

void PopulationManager::mutate_swap(std::vector<int>& chromosome) {
	std::uniform_int_distribution<> distribution(0, chromosome_size - 1);
	int first = distribution(rand_gen);
	int second = distribution(rand_gen);
	while ( first == second )
		second = distribution(rand_gen);

	std::swap(chromosome.at(first), chromosome.at(second));
}
