#pragma once

#include "selectors/Selector.hpp"

#include "types.hpp"

class RouletteSelector : public Selector {
public:
	RouletteSelector(IndividualPtrVec& population, std::mt19937& rand_gen);

	IndividualPtrVec select_n(unsigned int n) override;

};
