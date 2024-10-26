#pragma once

#include "PopulationManager.hpp"
#include "CSVLogger.hpp"

class TSPSolver {
public:
	TSPSolver(unsigned int pop_size, unsigned int gen_count, float cross_prob, float mut_prob,
	          const std::string& input_filename, const std::string& output_filename);

	void solve();

private:
	Loader data_loader;
	PopulationManager pmgr;
	CSVLogger logger;
	const unsigned int gen_count;

	void log_stats_to_file(FitnessStats& stats, int generation_num);

};
