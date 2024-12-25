#include "selectors/TournamentSelector.hpp"

#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <set>
#include <chrono>
#include <cassert>

#include "constants.hpp"

TournamentSelector::TournamentSelector(Population &population, const Settings &settings, std::mt19937 &rand_gen)
	: Selector(population, settings, rand_gen) {
}

// Selects more than n, depending on how many elite individuals were selected.
// Makes it easier to leave them during creation of next generation of individuals.
std::vector<Individual> TournamentSelector::select_n(const unsigned int n) {
	std::multimap pop_left(population.get_population().begin(), population.get_population().end());
	std::vector<Individual> result;
	result.reserve(n + settings.elite_sz);
	int n_selected_elite = 0;

	while ( result.size() < n ) {
		auto distribution = std::uniform_int_distribution<>(0, settings.pop_size - result.size() - 1);
		std::set<Individual> round_selected;

		while ( round_selected.size() < settings.tour_sz ) {
			int rand_idx = distribution(rand_gen);
			auto rand_individual = *std::next(pop_left.begin(), rand_idx);
			assert(("[Tournament selector] Zero-value fitness!", rand_individual.first != 0.f));
			round_selected.insert(rand_individual);
		}

		auto round_best = *round_selected.begin();
		if ( round_best.second.is_elite )
			++n_selected_elite;

		result.emplace_back(round_best);

		bool erased = false;
		std::erase_if(pop_left, [&round_best, &erased](const Individual& i) {
			if ( i.first == round_best.first && i.second.chromosome == round_best.second.chromosome && !erased ) {
				erased = true;
				return true;
			}

			return false;
		});
	}

	// Select additional 'n_selected_elite' random individuals
	for ( int i = 0; i < n_selected_elite; ++i ) {
		auto distribution = std::uniform_int_distribution<>(0, pop_left.size() - i - 1);
		auto rand_additional_individual = std::next(pop_left.begin(), distribution(rand_gen));
		result.emplace_back(*rand_additional_individual);
		pop_left.erase(rand_additional_individual);
	}

	return {result.begin(), result.end()};
}
