#include "../include/genetic_algorithm.h"
#include "../include/instance.h"
#include "evaluate.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <ranges>
#include <utility>
#include <vector>

void GeneticAlgorithm::evaluatePopulation(std::vector<individual_t>& population, std::vector<std::vector<float>> const& distanceMatrix, const int& truckCapacity){
    for (auto individual : population) {
        individual.fitnessValue = evaluateSolution(individual.chromosome, distanceMatrix, truckCapacity);
    }
}

void GeneticAlgorithm::initializePopulation(){
    std::vector<const Node*> randomChromosome(this->problemInstance.nodes.size()-1);

    int i=0;
    for (const Node& node : this->problemInstance.nodes | std::views::drop(1)) {
        randomChromosome[i] = &node;
        i++;
    }

    std::random_device rd;
    std::mt19937 g(rd());

    this->population.resize(this->parameters.generations);
    this->population[0].resize(this->parameters.populationSize);
    for (individual_t& individual : this->population[0]) {
        individual_t tmp{randomChromosome, 0};
        individual = tmp;
        std::shuffle(randomChromosome.begin(), randomChromosome.end(), g);
    }

    // DIVIDE INTO ROUTES

    for (individual_t& individual : this->population[0]) {
        int currentDemand = 0;
        for (int i=0; i<individual.chromosome.size(); i++) { //const Node* node : individual.chromosome
            currentDemand += individual.chromosome[i]->demand;
            if (currentDemand > this->problemInstance.getCapacity()) {
                individual.chromosome.emplace(individual.chromosome.begin()+i, this->sepNode);
                currentDemand = 0;
            }
        }
    }

    // EVALUATE INITIAL POPULATION
    evaluatePopulation(this->population[0], this->problemInstance.distanceMatrix, this->problemInstance.getCapacity());
    
    for (const auto& individual : this->population[0]) {
        for (const auto& node : individual.chromosome) {
            std::cout << node->id << " ";
        }
        std::cout << std::endl;
    }
}

int roulette(){
    
    return 0;
}

// int tournament(int tourSize){}

bool individualCompare(const individual_t& lhs, const individual_t& rhs){
    return lhs.fitnessValue < rhs.fitnessValue;
}

std::vector<individual_t> GeneticAlgorithm::selectGeneration(std::vector<individual_t> selectionPool){
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(selectionPool.begin(), selectionPool.end(), g);
    std::sort(selectionPool.begin(), selectionPool.end(), individualCompare);

    selectionPool.erase(selectionPool.begin() + this->parameters.populationSize, selectionPool.end());
    return selectionPool;
}

std::pair<individual_t, individual_t> GeneticAlgorithm::selectParents(std::vector<individual_t> selectionPool){
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(selectionPool.begin(), selectionPool.end(), g);
    selectionPool.erase(selectionPool.begin() + this->parameters.tournamentSize, selectionPool.end());

    std::sort(selectionPool.begin(), selectionPool.end(), individualCompare);

    return std::make_pair(selectionPool[0], selectionPool[1]);
}