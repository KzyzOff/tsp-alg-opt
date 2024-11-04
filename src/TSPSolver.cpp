#include "TSPSolver.hpp"

#include <sstream>

TSPSolver::TSPSolver(const Settings &settings)
	: settings(settings),
	  data_loader(settings.input_file),
	  pmgr(settings, data_loader.get_locations()) {
	logger.set_col_number(5);
}

void TSPSolver::solve_n_times(const unsigned int n, const unsigned int n_threads) {
	FitnessStats stats {};
	set_output_filename(settings.input_file.c_str(), 0);
	log_stats_to_file(stats, 0);
	for ( int i = 0; i < settings.gen_count; ++i ) {
		pmgr.advance_population();
		stats = pmgr.calc_fitness_stats();
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

void TSPSolver::set_output_filename(const std::filesystem::path &filename, unsigned int n) {
	std::ostringstream output_filename;
	output_filename << filename.stem().string() << "_p" << settings.pop_size << "_g" << settings.gen_count
			<< "_px" << static_cast<char>(settings.cross_t) << '-' << settings.cross_prob
			<< "_pm" << static_cast<char>(settings.mut_t) << '-' << settings.mut_prob
			<< "_n" << n << ".csv";
	logger.set_output_file(output_filename.str());
}

void TSPSolver::log_stats_to_file(FitnessStats &stats, int generation_num) {
	logger.add(0, generation_num);
	logger.add(1, stats.best);
	logger.add(2, stats.mean);
	logger.add(2, stats.worst);
	logger.commit_row();
}
