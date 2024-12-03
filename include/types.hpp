#pragma once

#include <vector>
#include <memory>
#include <filesystem>

namespace tsp_t
{
	struct Location {
		int n;
		float x, y;
	};

	enum class InitType : char {
		RANDOM = 'r',
		GREEDY = 'g'
	};

	enum class CrossoverType : char {
		NONE = 'n',
		OX = 'o',
		PMX = 'p'
	};

	enum class MutationType : char {
		NONE = 'n',
		INVERSE = 'i',
		SWAP = 's'
	};

	enum class SelectionType : char {
		TOURNAMENT = 't',
		ROULETTE = 'r'
	};

	struct Settings {
		InitType init_t;
		CrossoverType cross_t;
		MutationType mut_t;
		SelectionType sel_t;
		float cross_prob;
		float mut_prob;
		int pop_size;
		int gen_count;
		int elite_sz;
		std::filesystem::path input_file;
	};

	struct FitnessStats {
		float worst;
		float best;
		float mean;
	};

	using LocationsPtr = std::shared_ptr<std::vector<Location>>;
	using Individual = std::pair<float, std::vector<int>>;
}
