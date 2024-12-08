#pragma once

#include <string>
#include <vector>

#include "types.hpp"

float calc_distance(float x1, float y1, float x2, float y2);

std::string vec2str(const std::vector<int>& vec);

void mt_run(tsp_t::Settings settings, int id);
