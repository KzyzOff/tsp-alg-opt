#pragma once

#include "cross_operators/CrossOperator.hpp"

#include "types.hpp"

class OXOPerator : public CrossOperator {
public:
	OXOPerator(IndividualPtrVec &population, std::mt19937 &rand_gen, unsigned int chosen_parent_count);

	std::pair<Individual, Individual> cross(const Individual &parent1, const Individual &parent2) override;

private:
	Individual cross_parents(const Individual &parent1, const Individual &parent2,
	                         int cut_start, int cut_end);
};
