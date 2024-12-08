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

	struct EliteFlaggedChromosome {
		std::vector<int> chromosome;
		bool is_elite;

		// bool operator==(const EliteFlaggedChromosome& efc) {
		// 	return this->chromosome == efc.chromosome;
		// }
	};

	using LocationsPtr = std::shared_ptr<std::vector<Location>>;
	using Individual = std::pair<float, EliteFlaggedChromosome>;

	inline bool operator<(const Individual& lhs, const Individual& rhs) {
		if (lhs.first != rhs.first) {
			return lhs.first < rhs.first;
		}

		return lhs.second.chromosome < rhs.second.chromosome;
	}

	// struct IndividualComparator {
	// 	bool operator<(const Individual& lhs, const Individual& rhs) const {
	// 		if (lhs.first != rhs.first) {
	// 			return lhs.first < rhs.first;
	// 		}
	//
	// 		return lhs.second.chromosome < rhs.second.chromosome;
	// 	}
	// };
}
