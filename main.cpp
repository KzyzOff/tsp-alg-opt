#include "types.hpp"
#include "TSPSolver.hpp"

int main() {
	const Settings settings {
		.init_t = InitType::RANDOM,
		.cross_t = CrossoverType::PMX,
		.mut_t = MutationType::SWAP,
		.sel_t = SelectionType::TOURNAMENT,
		.cross_prob = .5f,
		.mut_prob = .1f,
		.pop_size = 1000,
		.gen_count = 10000,
		.elite_sz = 20,
		.input_file = std::filesystem::path("berlin52.tsp")
	};

	TSPSolver solver(settings);
	solver.solve(std::pow(10, 4), settings.gen_count);

	return 0;
}
