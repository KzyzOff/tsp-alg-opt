#include "cross_operators/PMXOperator.hpp"

#include <set>
#include <map>
#include <algorithm>

PMXOperator::PMXOperator(Population &population, std::mt19937 &rand_gen, unsigned int chosen_parent_count)
	: CrossOperator(population, rand_gen, chosen_parent_count) {
}

std::pair<Individual, Individual> PMXOperator::cross(const Individual &parent1, const Individual &parent2) {
	std::uniform_int_distribution<> distribution(0, chromosome_size - 1);
	int cut_start = distribution(rand_gen);
	int cut_end = distribution(rand_gen);
	while ( cut_start == cut_end )
		cut_end = distribution(rand_gen);
	if ( cut_start > cut_end )
		std::swap(cut_start, cut_end);

	Individual offspring1;
	Individual offspring2;
	offspring1.second.resize(chromosome_size);
	offspring2.second.resize(chromosome_size);
	std::ranges::fill(offspring1.second, -1);
	std::ranges::fill(offspring2.second, -1);
	for ( int i = cut_start; i < cut_end; ++i ) {
		offspring1.second.at(i) = parent1.second.at(i);
		offspring2.second.at(i) = parent2.second.at(i);
	}

	map_remaining_pmx(parent1, parent2, offspring1, cut_start, cut_end);
	map_remaining_pmx(parent2, parent1, offspring2, cut_start, cut_end);

	return {offspring1, offspring2};
}

void PMXOperator::map_remaining_pmx(const Individual &parent1, const Individual &parent2, Individual &offspring,
                                    int cut_start, int cut_end) {
	using std::ranges::find;
	// <parent1 gene, parent2 gene>
	std::map<int, int> parent1_to_parent2_map;
	std::set<int> used_genes;

	for ( int i = cut_start; i < cut_end; ++i ) {
		parent1_to_parent2_map.insert({parent1.second.at(i), parent2.second.at(i)});
		used_genes.insert(parent1.second.at(i));
	}

	for ( int i = 0; i < chromosome_size; ++i ) {
		if ( offspring.second.at(i) != -1 ) continue;

		int gene_to_insert = parent2.second.at(i);
		while ( used_genes.contains(gene_to_insert) ) {
			gene_to_insert = parent1_to_parent2_map.at(gene_to_insert);
		}
		offspring.second.at(i) = gene_to_insert;
		used_genes.insert(gene_to_insert);
	}
}
