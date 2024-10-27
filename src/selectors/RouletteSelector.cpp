#include "selectors/RouletteSelector.hpp"

#include <unordered_set>

#include "constants.hpp"

RouletteSelector::RouletteSelector(IndividualPtrVec &population, std::mt19937 &rand_gen)
	: Selector(population, rand_gen) {}

IndividualPtrVec RouletteSelector::select_n(unsigned int n) {
	auto accumulator = [](float acc, const std::shared_ptr<Individual> i) {
		return acc + i->fitness;
	};
	float total_fitness = std::accumulate(population.begin(), population.end(), 0.f, accumulator);
	std::vector<float> cumulative_fitness(population.size());
	cumulative_fitness.at(0) = population.at(0)->fitness / total_fitness * ROULETTE_SCALER;
	for ( int i = 1; i < population.size(); ++i ) {
		cumulative_fitness.at(i) = cumulative_fitness.at(i - 1) + ROULETTE_SCALER * (
			                           population.at(i)->fitness / total_fitness);
	}

	std::uniform_real_distribution<float> distribution(0.f, 1.f * ROULETTE_SCALER);
	std::unordered_set<int> picked_individuals_indices;
	while ( picked_individuals_indices.size() < n ) {
		float picked_value = distribution(rand_gen);
		int index = 0;
		while ( index < population.size() && picked_value > cumulative_fitness.at(index) ) {
			++index;
		}
		picked_individuals_indices.insert(index);
	}

	std::vector<std::shared_ptr<Individual>> result;
	result.reserve(n);
	for ( auto i : picked_individuals_indices ) {
		result.push_back(population.at(i));
	}

	return result;
}
