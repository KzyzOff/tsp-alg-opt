#include "selectors/TournamentSelector.hpp"

#include <unordered_set>
#include <algorithm>

#include "constants.hpp"

TournamentSelector::TournamentSelector(IndividualPtrVec& population, std::mt19937 &rand_gen)
	: Selector(population, rand_gen) {
}

IndividualPtrVec TournamentSelector::select_n(const unsigned int n) {
	auto comparator = [](const IndividualPtr &i1, const IndividualPtr &i2) {
		return i1->fitness < i2->fitness;
	};

	auto distribution = std::uniform_int_distribution<>(0, population.size() - 1);
	std::unordered_set<std::shared_ptr<Individual> > result;
	while ( result.size() < n ) {
		std::unordered_set<std::shared_ptr<Individual> > tournament_round_selected;
		while ( tournament_round_selected.size() < TOURNAMENT_SIZE ) {
			tournament_round_selected.insert(population.at(distribution(rand_gen)));
		}

		auto best = std::ranges::min_element(tournament_round_selected, comparator);
		result.insert(*best);
	}

	return {result.begin(), result.end()};
}
