#include "selectors/TournamentSelector.hpp"

#include <unordered_set>
#include <algorithm>
#include <set>

#include "constants.hpp"

TournamentSelector::TournamentSelector(Population& population, const Settings& settings, std::mt19937 &rand_gen)
	: Selector(population, settings, rand_gen) {
}

// Selects more than n, depending on how many elite individuals were selected.
// Makes it easier to leave them during creation of next generation of individuals.
std::vector<FlaggedIndividual> TournamentSelector::select_n(const unsigned int n) {
	std::multimap<float, std::vector<int>> pop_left {population.get_population().begin(), population.get_population().end()};
	std::vector<FlaggedIndividual> result;
	result.reserve(n);
	auto elite_individuals = population.get_n_best(settings.elite_sz);
	int n_selected_elite = 0;
	while ( result.size() < n + n_selected_elite ) {
		auto distribution = std::uniform_int_distribution<>(0, settings.pop_size - result.size() - 1);
		std::set<Individual> tournament_round_selected;
		while ( tournament_round_selected.size() < TOURNAMENT_SIZE ) {
			auto random_individual = std::next(pop_left.begin(), distribution(rand_gen));
			tournament_round_selected.insert(*random_individual);
		}

		auto round_best = *tournament_round_selected.begin();

		bool is_elite = false;
		for ( const auto& e : elite_individuals ) {
			if ( population.same_individuals(e, round_best) ) {
				is_elite = true;
				++n_selected_elite;
				break;
			}
		}

		result.push_back({round_best, is_elite});

		auto to_remove_iters = pop_left.equal_range(round_best.first);
		for ( auto i = to_remove_iters.first; i != to_remove_iters.second; ++i ) {
			if ( i->second == round_best.second ) {
				pop_left.erase(i);
				break;
			}
		}
	}

	return {result.begin(), result.end()};
}
