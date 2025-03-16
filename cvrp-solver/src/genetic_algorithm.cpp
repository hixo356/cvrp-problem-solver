#include "../include/genetic_algorithm.h"
#include "instance.h"
#include <algorithm>
#include <iostream>
#include <random>

void GeneticAlgorithm::initializePopulation(int const& populationSize, int const& generations, std::vector<Node>& nodes){
    std::vector<Node*> randomChromosome(nodes.size());

    int i=0;
    for (Node& node : nodes) {
        randomChromosome[i] = &node;
        i++;
    }

    std::random_device rd;
    std::mt19937 g(rd());

    this->population.resize(generations);
    this->population[0].resize(populationSize);
    for (individual_t& individual : this->population[0]) {
        individual_t tmp{randomChromosome, 0};
        individual = tmp;
        std::shuffle(randomChromosome.begin(), randomChromosome.end(), g);
    }

    for (const auto& individual : this->population[0]) {
        for (const auto& node : individual.chromosome) {
            std::cout << node->id << " ";
        }
        std::cout << std::endl;
    }
}