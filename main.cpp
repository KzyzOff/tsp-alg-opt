#include <thread>

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
		.cross_prob = .5f,
		.mut_prob = .2f,
		.pop_size = 800,
		.gen_count = 10000,
		.elite_sz = 30,
		.input_file = std::filesystem::path("berlin52.tsp")
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
