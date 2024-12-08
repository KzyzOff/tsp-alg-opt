#pragma once

#include "selectors/Selector.hpp"

class TournamentSelector : public Selector {
public:
	TournamentSelector(Population& population, const Settings& settings, std::mt19937& rand_gen);

	std::vector<Individual> select_n(unsigned int n) override;

};
