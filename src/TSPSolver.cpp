#include "TSPSolver.hpp"

TSPSolver::TSPSolver(unsigned int pop_size, unsigned int gen_count, float cross_prob, float mut_prob,
                     const std::string &input_filename)
	: data_loader(input_filename),
	  pmgr(pop_size, gen_count, cross_prob, mut_prob, data_loader.get_locations()),
	  logger("test_output.csv", 6),
	  gen_count(gen_count) {

}

void TSPSolver::solve() {
	pmgr.generate_random_pop();
	FitnessStats stats = pmgr.calc_fitness_stats();
	log_stats_to_file(stats, 0);
	for ( int i = 0; i < gen_count - 1; ++i ) {
		pmgr.advance_population();
		log_stats_to_file(stats, i + 1);
	}
}

void TSPSolver::log_stats_to_file(FitnessStats& stats, int generation_num) {
	logger.add(0, generation_num);
	logger.add(1, stats.best);
	logger.add(2, stats.mean);
	logger.add(2, stats.worst);
	logger.commit_row();
}
