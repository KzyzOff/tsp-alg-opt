#include "TSPSolver.hpp"

#include <sstream>

#include "constants.hpp"

TSPSolver::TSPSolver(const Settings &settings)
	: settings(settings),
	  data_loader(settings.input_file),
	  pmgr(settings, data_loader) {
	logger.set_col_number(5);
}

void TSPSolver::solve(const uint64_t max_fitness_update_count, const unsigned int n_threads) {
	FitnessStats stats {};
	set_output_filename(settings.input_file.c_str(), 2);
	log_stats_to_file(stats, 0);
	int gen_number = 1;
	uint64_t current_fitness_update_count = 0;
	while ( current_fitness_update_count < max_fitness_update_count && gen_number <= settings.gen_count ) {
		pmgr.advance_population();
		stats = pmgr.calc_fitness_stats();
		log_stats_to_file(stats, gen_number);
		printf("[Generation %i] Best: %f, Mean: %f, Worst: %f; fitness_calc count: %llu\n",
		       gen_number, stats.best, stats.mean, stats.worst, current_fitness_update_count);
		++gen_number;
		current_fitness_update_count = pmgr.get_fitness_update_count();
	}

	const auto [fitness, individual] = pmgr.get_goat();
	std::ostringstream best_chromosome;
	for ( int i = 0; i < individual.chromosome.size(); ++i ) {
		best_chromosome << individual.chromosome.at(i);
		if ( i != individual.chromosome.size() - 1 )
			best_chromosome << "->";
	}

	printf("Best solution chromosome: %s\nBest fitness: %f", best_chromosome.str().c_str(), fitness);
}

void TSPSolver::set_output_filename(const std::filesystem::path &filename, unsigned int n) {
	std::ostringstream output_filename;
	output_filename << filename.stem().string() << "_p" << settings.pop_size
			<< "_esz" << settings.elite_sz
			<< "_tsz" << TOURNAMENT_SIZE
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
