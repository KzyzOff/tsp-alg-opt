#include <thread>
#include <mutex>

#include "constants.hpp"
#include "utils.hpp"
#include "types.hpp"
#include "TSPSolver.hpp"

#include "JSONBatchSimulator.hpp"

int main() {
	bool batch = true;

	if ( batch ) {
		const std::filesystem::path file("./recalculation.json");
		JSONBatchSimulator batch_simulator(file);
		// batch_simulator.solve_dataset("berlin52.tsp");
		// batch_simulator.solve_dataset("berlin_modified.tsp");
		// batch_simulator.solve_dataset("kroA100.tsp");
		// batch_simulator.solve_dataset("kroA150.tsp");
		batch_simulator.solve_dataset("fl417.tsp");
		batch_simulator.solve_dataset("ali535.tsp");
		batch_simulator.solve_dataset("gr666.tsp");

		return 0;
	}

	namespace fs = std::filesystem;

	const Settings settings {
		.init_t = InitType::RANDOM,
		.cross_t = CrossoverType::OX,
		.mut_t = MutationType::INVERSE,
		.sel_t = SelectionType::TOURNAMENT,
		.cross_prob = .4f,
		.mut_prob = .5f,
		.pop_size = 2000,
		.elite_sz = 200,
		.tour_sz = 200,
		.input_file = fs::path("kroA100.tsp")
	};
	int max_fitness_update_count = static_cast<int>(std::pow(10, 6));
	// int max_fitness_update_count = static_cast<int>(2 * std::pow(10, 6));

	// Skip the calculations if specified settings are already calculated
	fs::path resulting_filepath(SIMULATION_RESULTS_PATH / settings.input_file.stem());
	resulting_filepath += "/" + stringify_settings(settings) + "_n0.csv";

	if ( fs::exists(resulting_filepath) ) {
		printf("Sepcified settings are already calculated - skipping.\n");
		return 1;
	}

	const int n_simulations = 10;
	const int n_threads = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;
	threads.reserve(n_threads);
	std::mutex results_mutex;

	std::map<int, Individual> results;

	auto start_timestamp = std::chrono::steady_clock::now();

	for ( int i = 0; i < n_simulations; ++i ) {
		threads.emplace_back([&, max_fitness_update_count, i]() {
			Individual result = mt_run(settings, max_fitness_update_count, i);
			{
				std::lock_guard lock(results_mutex);
				results[i] = result;
			}
		});

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

	json_add_best(std::filesystem::path(SIMULATION_RESULTS_PATH) / settings.input_file.stem(),
	              stringify_settings(settings),
	              current_best);

	return 0;
}
