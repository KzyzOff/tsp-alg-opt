#pragma once

#include "Loader.hpp"
#include "PopulationManager.hpp"
#include "CSVLogger.hpp"

class TSPSolver {
public:
	explicit TSPSolver(const Settings& settings);

	void solve(uint64_t max_fitness_update_count, unsigned n_threads = 1);

private:
	const Settings &settings;
	Loader data_loader;
	PopulationManager pmgr;
	CSVLogger logger;

	void set_output_filename(const std::filesystem::path &filename, unsigned int n);
	void log_stats_to_file(FitnessStats& stats, int generation_num);

};
