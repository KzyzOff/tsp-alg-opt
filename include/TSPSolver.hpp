#pragma once

#include "PopulationManager.hpp"

class TSPSolver {
public:
	TSPSolver(unsigned int pop_size, unsigned int gen_count, float cross_prob, float mut_prob,
	          const std::string& input_filename);

	void solve();

private:
	Loader data_loader;
	PopulationManager pmgr;

};
