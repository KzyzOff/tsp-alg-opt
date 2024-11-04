#include "Population.hpp"

#include <algorithm>

#include "utils.hpp"

Population::Population(unsigned int pop_size, InitType init_type, const LocationsPtr locations, std::mt19937& rand_gen)
	: rand_gen(rand_gen),
	  locations(locations) {
	switch (init_type) {
		case InitType::RANDOM:
			random_init(pop_size);
			break;
		case InitType::GREEDY:
			greedy_init(pop_size);
			break;
	}
}

void Population::update_fitness(Individual &individual) {
	individual.fitness = 0.f;
	const int chromosome_size = locations->size();
	for ( size_t i = 0; i < chromosome_size; ++i ) {
		if ( i == chromosome_size - 1 ) {
			individual.fitness += calc_distance(locations->at(individual.chromosome.at(i) - 1).x,
												locations->at(individual.chromosome.at(i) - 1).y,
												locations->at(individual.chromosome.at(0) - 1).x,
												locations->at(individual.chromosome.at(0) - 1).y);
		} else {
			individual.fitness += calc_distance(locations->at(individual.chromosome.at(i) - 1).x,
												locations->at(individual.chromosome.at(i) - 1).y,
												locations->at(individual.chromosome.at(i + 1) - 1).x,
												locations->at(individual.chromosome.at(i + 1) - 1).y);
		}
	}
}

void Population::random_init(unsigned int pop_size) {
	population.clear();
	population.resize(pop_size);
	for ( size_t i = 0; i < pop_size; ++i ) {
		population.at(i) = std::make_shared<Individual>();

		population.at(i)->chromosome.resize(locations->size());
		for ( int x = 0; x < locations->size(); ++x ) {
			population.at(i)->chromosome.at(x) = locations->at(x).n;
		}
		std::ranges::shuffle(population.at(i)->chromosome, rand_gen);
		update_fitness(*population.at(i));
	}

	goat = *std::ranges::min_element(population, [](const IndividualPtr& i1, const IndividualPtr& i2) {
		return i1->fitness < i2->fitness;
	});
}

void Population::greedy_init(unsigned int pop_size) {

}
