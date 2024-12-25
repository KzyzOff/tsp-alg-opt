#pragma once

#include <filesystem>
#include <json.hpp>

#include "types.hpp"

class JSONBatchSimulator {
public:
	explicit JSONBatchSimulator(const std::filesystem::path& json_path);

	void solve_all();
	void solve_dataset(const std::filesystem::path& dataset);


private:
	nlohmann::json sim_suite;

	tsp_t::Settings json2settings(const nlohmann::json& json_settings, std::filesystem::path dataset);

	tsp_t::CrossoverType char2cross_t(std::string ch);
	tsp_t::MutationType char2mut_t(std::string ch);

};
