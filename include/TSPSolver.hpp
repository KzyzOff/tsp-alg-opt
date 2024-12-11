#pragma once

#include "Loader.hpp"
#include "PopulationManager.hpp"
#include "CSVLogger.hpp"

class TSPSolver {
public:
	explicit TSPSolver(Settings settings);

	Individual solve(uint64_t max_fitness_update_count, int run_id);

private:
	const Settings settings;
	Loader data_loader;
	PopulationManager pmgr;
	CSVLogger logger;

	void set_output_filename(unsigned int n);
	void log_stats_to_file(FitnessStats& stats, int generation_num);

};
