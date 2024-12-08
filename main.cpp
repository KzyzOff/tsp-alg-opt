#include <thread>

#include "utils.hpp"
#include "types.hpp"
#include "TSPSolver.hpp"

void runSimulation(int id) {
	std::cout << "Simulation " << id << " started on thread "
			  << std::this_thread::get_id() << std::endl;

	std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate work

	std::cout << "Simulation " << id << " finished on thread "
			  << std::this_thread::get_id() << std::endl;
}

int main() {
	const Settings settings {
		.init_t = InitType::RANDOM,
		.cross_t = CrossoverType::PMX,
		.mut_t = MutationType::SWAP,
		.sel_t = SelectionType::TOURNAMENT,
		.cross_prob = .5f,
		.mut_prob = .2f,
		.pop_size = 800,
		.gen_count = 10000,
		.elite_sz = 30,
		.input_file = std::filesystem::path("kroA100.tsp")
	};

	const int n_simulations = 10;
	const int n_threads = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;
	threads.reserve(n_threads);

	for ( int i = 0; i < n_simulations; ++i ) {
		threads.emplace_back(mt_run, settings, i);

		if ( threads.size() >= n_threads ) {
			for ( auto& thread : threads ) {
				thread.join();
			}
			threads.clear();
		}
	}

	for ( auto& thread : threads ) {
		thread.join();
	}

	return 0;
}
