#pragma once

#include "selectors/Selector.hpp"

#include "types.hpp"

class RouletteSelector : public Selector {
public:
	RouletteSelector(Population& population, std::mt19937& rand_gen);

	std::vector<Individual> select_n(unsigned int n) override;

};
