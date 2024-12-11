#pragma once

#include <string>
#include <vector>

#include "types.hpp"

float calc_distance(float x1, float y1, float x2, float y2);

std::string vec2str(const std::vector<int>& vec);

tsp_t::Individual mt_run(tsp_t::Settings settings, int id);

std::string stringify_settings(const tsp_t::Settings& settings);

void json_add_best(const std::filesystem::path& filepath, const std::string& settings_str, const tsp_t::Individual& best);
