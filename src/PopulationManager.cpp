#include "PopulationManager.hpp"

#include <algorithm>
#include <map>
#include <random>
#include <utility>
#include <set>
#include <unordered_set>
#include <functional>
#include <list>
#include <optional>

#include "cross_operators/OXOperator.hpp"
#include "cross_operators/PMXOperator.hpp"
#include "selectors/TournamentSelector.hpp"
#include "selectors/RouletteSelector.hpp"
#include "utils.hpp"

PopulationManager::PopulationManager(const Settings &settings, const LocationsPtr &locations)
	: settings(settings),
	  cross_pop_count(settings.pop_size * settings.cross_prob),
	  mut_pop_count(settings.pop_size * settings.cross_prob),
	  chromosome_size(locations->size()),
	  rand_gen(rand_dev()),
	  population(settings.pop_size, settings.init_t, locations, rand_gen),
	  mut_type() {
	switch ( settings.cross_t ) {
		case CrossoverType::OX:
			cross_operator = std::make_unique<OXOPerator>(population.get_population(), rand_gen, cross_pop_count);
			break;
		case CrossoverType::PMX:
			cross_operator = std::make_unique<PMXOperator>(population.get_population(), rand_gen, cross_pop_count);
			break;
		case CrossoverType::NONE:
			cross_operator = nullptr;
			break;
	}

	switch ( settings.sel_t ) {
		case SelectionType::TOURNAMENT:
			selector = std::make_unique<TournamentSelector>(population.get_population(), rand_gen);
			break;
		case SelectionType::ROULETTE:
			selector = std::make_unique<RouletteSelector>(population.get_population(), rand_gen);
			break;
	}

	goat_individual = *std::ranges::min_element(population.get_population(),
	                                            [](const IndividualPtr &i1, const IndividualPtr &i2) {
		                                            return i1->fitness < i2->fitness;
	                                            });
}

// TODO: add precalculated lookup table for distances
void PopulationManager::update_pop_fitness() {
	for ( auto &individual : population.get_population() ) {
		population.update_fitness(*individual);
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
		.worst = std::max_element(population.get_population().begin(), population.get_population().end(), comparator)->
		         get()->fitness,
		.best = std::min_element(population.get_population().begin(), population.get_population().end(), comparator)->
		        get()->fitness,
		.mean = std::accumulate(population.get_population().begin(), population.get_population().end(), 1.f,
		                        accumulator) / static_cast<float>(
			        population.get_population().size())
	};

	return result;
}

void PopulationManager::advance_population() {
	if ( selector != nullptr && cross_operator != nullptr ) {
		auto selected_parents = selector->select_n(cross_pop_count);
		std::vector<std::shared_ptr<Individual> > children;
		children.reserve(selected_parents.size());
		// Skips the last one if selected parents count from the population is odd
		for ( int i = 1; i < selected_parents.size(); i += 2 ) {
			auto [offspring1, offspring2] = cross_operator->cross(*selected_parents.at(i - 1), *selected_parents.at(i));
			children.push_back(std::make_shared<Individual>(offspring1));
			children.push_back(std::make_shared<Individual>(offspring2));
		}

		for ( size_t i = 0; i < children.size(); ++i ) {
			population.update_fitness(*children.at(i));
			selected_parents.at(i)->chromosome = children.at(i)->chromosome;
			selected_parents.at(i)->fitness = children.at(i)->fitness;
		}
	}

	mutate_population(mut_type);
}

void PopulationManager::mutate_population(MutationType mt) {
	switch ( mt ) {
		case MutationType::NONE:
			break;

		case MutationType::INVERSE: {
			std::set<std::shared_ptr<Individual> > selected_individuals;
			std::uniform_int_distribution<> distribution(0, settings.pop_size - 1);

			while ( selected_individuals.size() < mut_pop_count ) {
				int index = distribution(rand_gen);
				selected_individuals.insert(population.get_population().at(index));
			}

			for ( auto &i : selected_individuals ) {
				mutate_inverse(*i);
				population.update_fitness(*i);
			}

			break;
		}

		case MutationType::SWAP: {
			static std::optional<std::vector<int>> indices;
			if ( indices == std::nullopt ) {
				indices = std::vector<int>(settings.pop_size);
				for ( int i = 0; i < settings.pop_size; ++i ) {
					indices->at(i) = i;
				}
			}

			std::ranges::shuffle(*indices, rand_gen);
			for ( int i = 0; i < mut_pop_count; ++i ) {
				mutate_swap(*population.get_population().at(i));
				population.update_fitness(*population.get_population().at(i));
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
