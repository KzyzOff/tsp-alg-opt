#include "TSPSolver.hpp"

TSPSolver::TSPSolver(unsigned int pop_size, unsigned int gen_count, float cross_prob, float mut_prob,
                     const std::string &input_filename)
	: data_loader(input_filename),
	  pmgr(pop_size, gen_count, cross_prob, mut_prob, data_loader.get_locations()) {}

void TSPSolver::solve() {
	pmgr.generate_random_pop();
	pmgr.advance_population();
}
