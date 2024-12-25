#include "JSONBatchSimulator.hpp"

#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

#include "constants.hpp"
#include "utils.hpp"

JSONBatchSimulator::JSONBatchSimulator(const std::filesystem::path &json_path) {
	std::ifstream file(json_path);
	file >> sim_suite;
}

void JSONBatchSimulator::solve_dataset(const std::filesystem::path& dataset) {
	const int max_fitness_update_count = 2 * std::pow(10, 6);
	const int n_simulations = 10;
	const int n_threads = std::thread::hardware_concurrency();
	// const int n_threads = 1;

	const std::string dataset_name = dataset.stem().string();
	std::cout << "----- " << dataset_name << " -----\n";

	for ( const auto& ds : sim_suite[dataset_name] ) {
		tsp_t::Settings settings = json2settings(ds, dataset);
		std::string settings_str = stringify_settings(settings);

		// Skip the calculations if specified settings are already calculated
		std::filesystem::path resulting_filepath(SIMULATION_RESULTS_PATH / settings.input_file.stem());
		resulting_filepath += "/" + settings_str + "_n0.csv";

		if ( std::filesystem::exists(resulting_filepath) ) {
			printf("Sepcified settings are already calculated - skipping.\n");
			continue;
		}

		std::cout << "Currently simulating: " << settings_str << "\n";

		std::vector<std::thread> threads;
		threads.reserve(n_threads);
		std::mutex results_mutex;

		std::map<int, tsp_t::Individual> results;

		auto start_timestamp = std::chrono::steady_clock::now();
		for ( int i = 0; i < n_simulations; ++i ) {
			threads.emplace_back([&, max_fitness_update_count, i]() {
				tsp_t::Individual result = mt_run(settings, max_fitness_update_count, i);
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
		std::cout << settings_str << " completed in: " << (float) sim_duration / 1000.f << " seconds.\n";

		tsp_t::Individual current_best = results.begin()->second;
		for ( const auto &[index, individual] : results ) {
			if ( individual.first < current_best.first )
				current_best = individual;
		}

		json_add_best(std::filesystem::path(SIMULATION_RESULTS_PATH) / settings.input_file.stem(),
		              stringify_settings(settings),
		              current_best);
	}
}

tsp_t::CrossoverType JSONBatchSimulator::char2cross_t(const std::string ch) {
	auto cross_t = tsp_t::CrossoverType::NONE;
	switch ( *ch.begin() ) {
		case 'p':
			cross_t = tsp_t::CrossoverType::PMX;
			break;
		case 'o':
			cross_t = tsp_t::CrossoverType::OX;
			break;
		default: break;
	}

	return cross_t;
}

tsp_t::MutationType JSONBatchSimulator::char2mut_t(const std::string ch) {
	auto mut_t = tsp_t::MutationType::NONE;
	switch ( *ch.begin() ) {
		case 'i':
			mut_t = tsp_t::MutationType::INVERSE;
			break;
		case 's':
			mut_t = tsp_t::MutationType::SWAP;
			break;
		default: break;
	}

	return mut_t;
}

tsp_t::Settings JSONBatchSimulator::json2settings(const nlohmann::json &json_settings, const std::filesystem::path dataset) {
	return {
		.init_t = tsp_t::InitType::RANDOM,
		.cross_t = char2cross_t(json_settings["cross_t"].get<std::string>()),
		.mut_t = char2mut_t(json_settings["mut_t"].get<std::string>()),
		.sel_t = tsp_t::SelectionType::TOURNAMENT,
		.cross_prob = json_settings["cross_prob"],
		.mut_prob = json_settings["mut_prob"],
		.pop_size = json_settings["pop_sz"],
		.elite_sz = json_settings["elite_sz"],
		.tour_sz = json_settings["tour_sz"],
		.input_file = dataset
	};
}
