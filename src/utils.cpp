#include "utils.hpp"

#include <cmath>
#include <sstream>
#include <fstream>
#include <json.hpp>

#include "constants.hpp"
#include "Greedy.hpp"
#include "TSPSolver.hpp"

float calc_distance(float x1, float y1, float x2, float y2) {
	float dx = std::abs(x1 - x2);
	float dy = std::abs(y1 - y2);

	return std::sqrt(dx * dx + dy * dy);
}

std::string vec2str(const std::vector<int> &vec, const std::string &separator = "") {
	std::ostringstream result;
	for ( size_t i = 0; i < vec.size(); ++i ) {
		result << vec.at(i);
		if ( i != vec.size() - 1 )
			result << separator;
	}
	// for ( auto el : vec ) {
	// 	result << el << separator;
	// }

	return result.str();
}

tsp_t::Individual mt_run(const Settings settings, int max_fitness_update_count, int id) {
	TSPSolver s(settings);
	tsp_t::Individual best = s.solve(max_fitness_update_count, id);
	// printf("Simulation %i has been completed.\n", id);

	return best;
}

std::string stringify_settings(const Settings &settings) {
	std::ostringstream output_filename;
	output_filename
			<< "p" << settings.pop_size
			<< "_esz" << settings.elite_sz
			<< "_tsz" << settings.tour_sz
			<< "_px" << static_cast<char>(settings.cross_t) << '-' << settings.cross_prob
			<< "_pm" << static_cast<char>(settings.mut_t) << '-' << settings.mut_prob;

	return output_filename.str();
}

void json_add_best(const std::filesystem::path &filepath, const std::string &settings_str, const Individual &best) {
	std::filesystem::path full_filepath(filepath);
	full_filepath += "/best_chromosomes.json";
	std::ifstream input_file(full_filepath);
	nlohmann::json json_data;

	if ( input_file.is_open() ) {
		try {
			input_file >> json_data;
		} catch ( ... ) {
			json_data = nlohmann::json::object();
		}
	} else {
		json_data = nlohmann::json::object();
	}
	input_file.close();

	if ( json_data.contains(settings_str) ) {
		std::cout << "File already contains this key: " << settings_str << "\n";
		return;
	}

	json_data[settings_str]["fitness"] = best.first;
	json_data[settings_str]["chromosome"] = vec2str(best.second.chromosome, ", ");

	std::ofstream output_file(full_filepath);
	output_file << json_data.dump(4);
}

void greedy_sim(const std::filesystem::path &dataset_path) {
	Loader loader(dataset_path);
	Greedy simulator(loader.get_locations(), loader.get_lookup_graph());
	std::filesystem::path output_path(SIMULATION_RESULTS_PATH);
	output_path += dataset_path.stem().string() + "/greedy.csv";
	CSVLogger logger;
	logger.set_output_file(output_path);
	logger.set_col_number(3);

	Individual best = simulator.get_solution(1);
	logger.add(0, 1);
	logger.add(1, best.first);
	logger.commit_row();

	for ( int i = 1; i < loader.get_locations()->size(); ++i ) {
		Individual solution = simulator.get_solution(i);
		logger.add(0, i + 1);
		logger.add(1, solution.first);
		logger.commit_row();

		if ( best.first > solution.first )
			best = solution;
	}

	logger.add(2, vec2str(best.second.chromosome, ", "));
	logger.commit_row();
}

void random_sim(const std::filesystem::path& dataset_path) {
	// TODO: implement this method
}
