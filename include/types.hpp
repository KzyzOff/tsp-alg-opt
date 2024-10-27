#pragma once

#include <memory>

namespace tsp_t
{
	struct Location {
		int n;
		float x, y;
	};

	enum class InitType {
		RANDOM,
		GREEDY
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
