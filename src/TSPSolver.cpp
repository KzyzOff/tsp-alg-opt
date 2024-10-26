#include "TSPSolver.hpp"

#include <iostream>
#include <sstream>

TSPSolver::TSPSolver(unsigned int pop_size, unsigned int gen_count, float cross_prob, float mut_prob,
                     const std::string& input_filename, const std::string& output_filename)
	: data_loader(input_filename),
	  pmgr(pop_size, gen_count, cross_prob, mut_prob, data_loader.get_locations()),
	  logger(output_filename, 5),
	  gen_count(gen_count) {

}

void TSPSolver::solve() {
	pmgr.generate_random_pop();
	FitnessStats stats {};
	log_stats_to_file(stats, 0);
	for ( int i = 0; i < gen_count; ++i ) {
		pmgr.advance_population();
		stats  = pmgr.calc_fitness_stats();
		log_stats_to_file(stats, i + 1);
		printf("[Generation %i] Best: %f, Mean: %f, Worst: %f\n", i, stats.best, stats.mean, stats.worst);
	}

	auto best_solution = pmgr.get_goat();
	std::ostringstream best_chromosome;
	for ( int i = 0; i < best_solution->chromosome.size(); ++i ) {
		best_chromosome << best_solution->chromosome.at(i);
		if ( i != best_solution->chromosome.size() - 1 )
			best_chromosome << "->";
	}

	printf("Best solution chromosome: %s", best_chromosome.str().c_str());
}

void TSPSolver::log_stats_to_file(FitnessStats& stats, int generation_num) {
	logger.add(0, generation_num);
	logger.add(1, stats.best);
	logger.add(2, stats.mean);
	logger.add(2, stats.worst);
	logger.commit_row();
}
