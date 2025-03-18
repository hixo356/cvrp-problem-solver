#include "../include/genetic_algorithm.h"
#include "../include/instance.h"
#include "evaluate.h"
#include <algorithm>
#include <climits>
#include <iostream>
#include <numeric>
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

    // go through all nodes except first, which is the depot
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

// int roulette(){
    
//     return 0;
// }

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

struct findById {
    int id;
    bool operator()(const Node* node) const {
        return node->id == id;
    }
};

int findNodeIdxById(std::vector<const Node*> vec, const int& id){
    int i=0;
    while (i<vec.size() && vec[i]->id != id ) {
        i++;
    }
    return i;
}

std::pair<individual_t, individual_t> GeneticAlgorithm::crossover(std::vector<const Node*> const& parent1, std::vector<const Node*> const& parent2){
    const int chromosomeSize = parent1.size();
    int p = 0, q = 0;
    Node emptyNode;
    Node* emptyNodePtr = &emptyNode;

    individual_t offspring1;
    offspring1.chromosome.resize(chromosomeSize, emptyNodePtr);
    individual_t offspring2;
    offspring2.chromosome.resize(chromosomeSize, emptyNodePtr);

    // get two randim unique points for dividing genes
    std::random_device rd;
    std::mt19937 g(rd());
    std::vector<int> ints(chromosomeSize);
    std::iota(ints.begin(), ints.end(), 0);
    std::shuffle(ints.begin(), ints.end(), g);
    if (ints[0] < ints[1]) {
        p = ints[0];
        q = ints[1];
    } else {
        p = ints[1];
        q = ints[0];
    }
    p = 3;
    q = 6;

    // replace middle parts
    std::copy(parent1.begin()+p, parent1.begin()+q, offspring2.chromosome.begin()+p);
    std::copy(parent2.begin()+p, parent2.begin()+q, offspring1.chromosome.begin()+p);

    // fill the rest accordingly
    // offspring1
    int i=0;
    while (i < chromosomeSize) {
        // skip middle fragment
        if (i == p) {
            i = q;
        }

        bool found = true;
        int foundIdx = INT_MAX;
        int wantedId = parent1[i]->id;
        int idx = findNodeIdxById(offspring1.chromosome, wantedId);
        if (idx == chromosomeSize) found = false;
        while (found) {
            foundIdx = idx;
            wantedId = parent1[idx]->id;
            idx = findNodeIdxById(offspring1.chromosome, wantedId);
            if (idx == chromosomeSize) found = false;
        }

        if (foundIdx >= chromosomeSize) {
            offspring1.chromosome[i] = parent1[i];
        } else {
            offspring1.chromosome[i] = parent1[foundIdx];
        }
        i++;
    }

    // offspring2
    i = 0;
    while (i < chromosomeSize) {
        // skip middle fragment
        if (i == p) {
            i = q;
        }

        bool found = true;
        int foundIdx = INT_MAX;
        int wantedId = parent2[i]->id;
        int idx = findNodeIdxById(offspring2.chromosome, wantedId);
        if (idx == chromosomeSize) found = false;
        while (found) {
            foundIdx = idx;
            wantedId = parent2[idx]->id;
            idx = findNodeIdxById(offspring2.chromosome, wantedId);
            if (idx == chromosomeSize) found = false;
        }

        if (foundIdx >= chromosomeSize) {
            offspring2.chromosome[i] = parent2[i];
        } else {
            offspring2.chromosome[i] = parent2[foundIdx];
        }
        i++;
    }

    return std::make_pair(offspring1, offspring2);
}