#include "types.hpp"
#include "TSPSolver.hpp"

int main() {
	Settings settings {
		.init_t = InitType::RANDOM,
		.cross_t = CrossoverType::PMX,
		.mut_t = MutationType::INVERSE,
		.sel_t = SelectionType::TOURNAMENT,
		.cross_prob = .6f,
		.mut_prob = .2f,
		.pop_size = 150,
		.gen_count = 10000,
		.input_file = std::filesystem::path("berlin52.tsp")
	};

	TSPSolver solver(settings);
	solver.solve_n_times();

	return 0;
}
