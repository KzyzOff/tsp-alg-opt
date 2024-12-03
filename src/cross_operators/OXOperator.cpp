#include "cross_operators/OXOperator.hpp"

#include <set>

OXOPerator::OXOPerator(Population &population, std::mt19937 &rand_gen, const unsigned int chosen_parent_count)
	: CrossOperator(population, rand_gen, chosen_parent_count) {
}

std::pair<Individual, Individual> OXOPerator::cross(const Individual &parent1, const Individual &parent2) {
	std::uniform_int_distribution<int> distribution(0, chromosome_size - 1);
	int cut_start = distribution(rand_gen);
	int cut_end = distribution(rand_gen);
	while ( cut_start == cut_end )
		cut_end = distribution(rand_gen);
	if ( cut_start > cut_end )
		std::swap(cut_start, cut_end);

	auto offspring1 = cross_parents(parent1, parent2, cut_start, cut_end);
	auto offspring2 = cross_parents(parent2, parent1, cut_start, cut_end);

	return {offspring1, offspring2};
}

Individual OXOPerator::cross_parents(const Individual &parent1, const Individual &parent2,
                                     const int cut_start, const int cut_end) {
	const std::set<int> used_genes(parent1.second.begin() + cut_start, parent1.second.begin() + cut_end);
	std::vector<int> remaining_genes;
	remaining_genes.resize(chromosome_size - (cut_end - cut_start));
	int remaining_idx = 0;
	for ( int i = 0; i < chromosome_size; ++i ) {
		if ( used_genes.contains(parent2.second.at(i)) ) continue;

		remaining_genes.at(remaining_idx) = parent2.second.at(i);
		++remaining_idx;
	}

	Individual offspring;
	offspring.second.reserve(chromosome_size);

	for ( int i = 0; i < remaining_genes.size(); ++i ) {
		if ( i == cut_start ) {
			offspring.second.insert(offspring.second.end(), parent1.second.begin() + cut_start,
			                            parent1.second.begin() + cut_end);
		}

		offspring.second.push_back(remaining_genes.at(i));
	}

	return offspring;
}
