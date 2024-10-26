#pragma once

#include <memory>

namespace tsp_t
{
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
}
