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
		std::filesystem::path input_file;
	};

	struct Individual {
		std::vector<int> chromosome;
		float fitness = 0.f;
	};

	struct FitnessStats {
		float worst;
		float best;
		float mean;
	};

	using IndividualPtr = std::shared_ptr<Individual>;
	using IndividualPtrVec = std::vector<std::shared_ptr<Individual>>;
	using LocationsPtr = std::shared_ptr<std::vector<Location>>;
}
