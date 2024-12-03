#pragma once

#include "selectors/Selector.hpp"

class TournamentSelector : public Selector {
public:
	TournamentSelector(Population& population, const Settings& settings, std::mt19937& rand_gen);

	std::vector<FlaggedIndividual> select_n(unsigned int n) override;

};
