#include <thread>
#include <mutex>

#include "constants.hpp"
#include "utils.hpp"
#include "types.hpp"
#include "TSPSolver.hpp"

int main() {
	const Settings settings {
		.init_t = InitType::RANDOM,
		.cross_t = CrossoverType::PMX,
		.mut_t = MutationType::SWAP,
		.sel_t = SelectionType::TOURNAMENT,
		.cross_prob = .6f,
		.mut_prob = .3f,
		.pop_size = 1000,
		.gen_count = 10000,
		.elite_sz = 5,
		.tour_sz = 5,
		.input_file = std::filesystem::path("berlin_modified.tsp")
	};

	const int n_simulations = 10;
	const int n_threads = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;
	threads.reserve(n_threads);
	std::mutex results_mutex;

	std::map<int, Individual> results;

	auto start_timestamp = std::chrono::steady_clock::now();

	for ( int i = 0; i < n_simulations; ++i ) {
		threads.emplace_back([&, i]() {
			Individual result = mt_run(settings, i);
			{
				std::lock_guard lock(results_mutex);
				results[i] = result;
			}
		});

		threads.emplace_back(mt_run, settings, i);

		if ( threads.size() >= n_threads ) {
			for ( auto &thread : threads ) {
				thread.join();
			}
			threads.clear();
		}
	}

	for ( auto &thread : threads ) {
		thread.join();
	}

	auto end_timestamp = std::chrono::steady_clock::now();
	auto sim_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_timestamp - start_timestamp).count();
	std::cout << "Simulation completed in: " << (float) sim_duration / 1000.f << " seconds.\n";

	Individual current_best = results.begin()->second;
	for ( const auto &[index, individual] : results ) {
		if ( individual.first < current_best.first )
			current_best = individual;
	}

	json_add_best_chormosome(std::filesystem::path(SIMULATION_RESULTS_PATH) / settings.input_file.stem(),
							 stringify_settings(settings),
	                         current_best.second.chromosome);

	return 0;
}
