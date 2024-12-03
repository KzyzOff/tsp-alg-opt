#pragma once

#include "cross_operators/CrossOperator.hpp"

class PMXOperator : public CrossOperator {
public:
	PMXOperator(Population &population, std::mt19937 &rand_gen, unsigned int chosen_parent_count);

	std::pair<Individual, Individual> cross(const Individual &parent1, const Individual &parent2) override;

private:
	void map_remaining_pmx(const Individual &parent1, const Individual &parent2, Individual &offspring,
						   int cut_start, int cut_end);

};
