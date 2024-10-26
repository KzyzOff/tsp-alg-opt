#pragma once

#include "selectors/Selector.hpp"

class TournamentSelector : public Selector {
public:
	TournamentSelector(IndividualPtrVec& population, std::mt19937& rand_gen);

	IndividualPtrVec select_n(unsigned int n) override;

};
