#include "../include/genetic_algorithm.h"
#include "../include/instance.h"
#include "../include/evaluate.h"
#include <algorithm>
#include <cfloat>
#include <chrono>
#include <climits>
#include <iostream>
#include <numeric>
#include <random>
#include <ranges>
#include <utility>
#include <vector>
#include <unordered_map>

void GeneticAlgorithm::evaluatePopulation(std::vector<individual_t>& population){
    for (auto& individual : population) {
        if (individual.fitnessValue != 0) {
            continue;
        }
        individual.fitnessValue = evaluateSolution(individual.chromosome, this->problemInstance.distanceMatrix, this->problemInstance.getCapacity(), this->evaluationCounter);
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

    this->population.resize(this->parameters.generations);
    this->population[0].resize(this->parameters.populationSize);
    for (individual_t& individual : this->population[0]) {
        individual_t tmp{randomChromosome, 0};
        individual = tmp;
        std::shuffle(randomChromosome.begin(), randomChromosome.end(), this->gen);
    }

    // DIVIDE INTO ROUTES

    for (individual_t& individual : this->population[0]) {
        int currentDemand = 0;
        for (int i=0; i<individual.chromosome.size(); i++) {
            currentDemand += individual.chromosome[i]->demand;
            if (currentDemand > this->problemInstance.getCapacity()) {
                individual.chromosome.emplace(individual.chromosome.begin()+i, this->sepNode);
                currentDemand = 0;
            }
        }
    }

    // EVALUATE INITIAL POPULATION
    evaluatePopulation(this->population[0]);
}

bool individualCompare(const individual_t& lhs, const individual_t& rhs){
    return lhs.fitnessValue < rhs.fitnessValue;
}

std::vector<individual_t> GeneticAlgorithm::selectGeneration(std::vector<individual_t> selectionPool){
    std::shuffle(selectionPool.begin(), selectionPool.end(), this->gen);
    std::sort(selectionPool.begin(), selectionPool.end(), individualCompare);

    selectionPool.erase(selectionPool.begin() + this->parameters.populationSize, selectionPool.end());
    return selectionPool;
}

std::pair<individual_t, individual_t> GeneticAlgorithm::selectParents(std::vector<individual_t> selectionPool){
    std::shuffle(selectionPool.begin(), selectionPool.end(), this->gen);
    selectionPool.erase(selectionPool.begin() + this->parameters.tournamentSize, selectionPool.end());

    std::sort(selectionPool.begin(), selectionPool.end(), individualCompare);

    return std::make_pair(selectionPool[0], selectionPool[1]);
}

void GeneticAlgorithm::fixSolution(individual_t& individual){
    if (individual.chromosome[0]->id == -1) {
        individual.chromosome.erase(individual.chromosome.begin());
    }

    int i = 0;
    while (i < individual.chromosome.size()) {
        if (individual.chromosome[i]->id == -1) {
            i++;
            while (i < individual.chromosome.size() && individual.chromosome[i]->id == -1) {
                individual.chromosome.erase(individual.chromosome.begin() + i);
            }
            continue;
        }
        i++;
    }
}

std::pair<individual_t, individual_t> GeneticAlgorithm::crossover(std::vector<const Node*> const& parent1, std::vector<const Node*> const& parent2){
    const int chromosomeSize = parent1.size();
    int p = 0, q = 0;
    Node emptyNode;
    Node* emptyNodePtr = &emptyNode;

    individual_t offspring1;
    offspring1.chromosome.resize(chromosomeSize, emptyNodePtr);
    individual_t offspring2;
    offspring2.chromosome.resize(parent2.size(), emptyNodePtr);

    // get two randim unique points for dividing genes
    std::vector<int> ints(std::min(parent1.size(), parent2.size()));
    std::iota(ints.begin(), ints.end(), 0);
    std::shuffle(ints.begin(), ints.end(), this->gen);
    if (ints[0] < ints[1]) {
        p = ints[0];
        q = ints[1];
    } else {
        p = ints[1];
        q = ints[0];
    }
    // p = 3;
    // q = 6;

    // replace middle parts
    std::copy(parent1.begin()+p, parent1.begin()+q, offspring2.chromosome.begin()+p);
    std::copy(parent2.begin()+p, parent2.begin()+q, offspring1.chromosome.begin()+p);

    // fill the rest accordingly
    // offspring1
    std::unordered_map<int, size_t> geneIndexMap1;
    for (size_t i=0; i < offspring1.chromosome.size(); i++) {
        int _id = offspring1.chromosome[i]->id;
        if (_id != 0) {
            geneIndexMap1[_id] = i;
        }
    }

    int i=0;
    while (i < parent1.size()) {
        // skip middle fragment
        if (i == p) {
            i = q;
        }

        // std::cout << "Offspring1 i: " << i << std::endl;

        bool found = true;
        int foundIdx = i;
        int wantedId = parent1[i]->id;
        // int idx = findNodeIdxById(offspring1.chromosome, wantedId);
        // auto idx = geneIndexMap1.find(wantedId);
        // if (idx == chromosomeSize) found = false;
        // while (found) {
        //     std::cerr << "XD" <<std::endl;
        //     foundIdx = idx;
        //     wantedId = parent1[idx]->id;
        //     idx = findNodeIdxById(offspring1.chromosome, wantedId);
        //     if (idx == chromosomeSize) found = false;
        // }
        // if (idx == geneIndexMap1.end()) found = false;
        // while (found) {
        //     // std::cerr << "XD" <<std::endl;
        //     foundIdx = geneIndexMap1[wantedId];
        //     wantedId = parent1[foundIdx]->id;
        //     auto idx = geneIndexMap1.find(wantedId);
        //     if (idx == geneIndexMap1.end()) found = false;
        // }

        while (geneIndexMap1.find(wantedId) != geneIndexMap1.end()){
            foundIdx = geneIndexMap1[wantedId];
            wantedId = parent1[foundIdx]->id;
            if (wantedId == -1) {
                break;
            }
        }

        if (foundIdx != chromosomeSize) {
            offspring1.chromosome[i] = parent1[foundIdx];
        } else {
            offspring1.chromosome[i] = parent1[foundIdx];
        }

        geneIndexMap1[offspring1.chromosome[i]->id] = i;
        i++;
    }

    // AKTUALIZOWAC HASH MAPE, TO MA BYC MAPA TEGO DZIECKA A NIE RODZICA, W SENSIE DWIE TRZEBA
    // RODZICA ZEBY BRAC NODE'Y, A DZIECKA ZEBY SPRAWDZAC CZY JUZ ISTNIEJA
    // I TAK DZIECKA UPDATE MUSI MIEC Z KAZDYM DODANIEM, WTEDY ESSA O(1) NIE

    // offspring2
    std::unordered_map<int, size_t> geneIndexMap2;
    for (size_t i=0; i < offspring2.chromosome.size(); i++) {
        int _id = offspring2.chromosome[i]->id;
        if (_id != 0) {
            geneIndexMap2[_id] = i;
        }
    }
    // for (size_t i=0; i < parent1.size(); i++) {
    //     geneIndexMap2[parent1[i]->id] = i;
    // }

    i = 0;
    while (i < parent2.size()) {
        // skip middle fragment
        if (i == p) {
            i = q;
        }

        // std::cout << "Offspring2 i: " << i << std::endl;

        bool found = true;
        int foundIdx = i;
        int wantedId = parent2[i]->id;
        // int idx = findNodeIdxById(offspring2.chromosome, wantedId);
        // if (idx == chromosomeSize) found = false;
        // while (found) {
        //     foundIdx = idx;
        //     wantedId = parent2[idx]->id;
        //     idx = findNodeIdxById(offspring2.chromosome, wantedId);
        //     if (idx == chromosomeSize) found = false;
        // }
        // auto idx = geneIndexMap2.find(wantedId);
        // if (idx == geneIndexMap2.end()) found = false;
        // while (found) {
        //     // std::cerr << "XD" <<std::endl;
        //     foundIdx = geneIndexMap2[wantedId];
        //     wantedId = parent2[foundIdx]->id;
        //     auto idx = geneIndexMap2.find(wantedId);
        //     if (idx == geneIndexMap2.end()) found = false;
        // }

        while (geneIndexMap2.find(wantedId) != geneIndexMap2.end()){
            foundIdx = geneIndexMap2[wantedId];
            wantedId = parent2[foundIdx]->id;
            if (wantedId == -1) {
                break;
            }
        }



        if (foundIdx != chromosomeSize) {
            offspring2.chromosome[i] = parent2[foundIdx];
        } else {
            offspring2.chromosome[i] = parent2[foundIdx];
        }

        geneIndexMap2[offspring2.chromosome[i]->id] = i;
        i++;
    }

    // FIX ANY POTENTIAL DOUBLE -1 NODES AND -1 A THE BEGINNING

    fixSolution(offspring1);
    fixSolution(offspring2);

    offspring1.fitnessValue = evaluateSolution(offspring1.chromosome, this->problemInstance.distanceMatrix, this->problemInstance.getCapacity(), this->evaluationCounter);
    offspring2.fitnessValue = evaluateSolution(offspring2.chromosome, this->problemInstance.distanceMatrix, this->problemInstance.getCapacity(), this->evaluationCounter);

    if (offspring1.chromosome.size() == 0 || offspring2.chromosome.size() == 0) {
        std::cerr << "SIZE 0" << std::endl;
    }
    return std::make_pair(offspring1, offspring2);
}

void GeneticAlgorithm::mutation(individual_t& individual){
    std::vector<int> ints(individual.chromosome.size()-1);
    std::iota(ints.begin(), ints.end(), 0);
    std::shuffle(ints.begin(), ints.end(), this->gen);
    std::swap(individual.chromosome[ints[0]], individual.chromosome[ints[1]]);

    fixSolution(individual);
    individual.fitnessValue = evaluateSolution(individual.chromosome, this->problemInstance.distanceMatrix, this->problemInstance.getCapacity(), this->evaluationCounter);
}

generationResult GeneticAlgorithm::summarizePopulation(std::vector<individual_t> const& population) const {
    float totalFitness = 0;
    float bestFitness = FLT_MAX;
    float worstFitness = 0;
    for (const auto& individual : population) {
        if (individual.fitnessValue < bestFitness) {
            bestFitness = individual.fitnessValue;
        } else if (individual.fitnessValue > worstFitness) {
            worstFitness = individual.fitnessValue;
        }
        totalFitness += individual.fitnessValue;
    }
    float averageFitness = totalFitness / population.size();

    return {bestFitness, averageFitness, worstFitness};
}

results_t GeneticAlgorithm::run(ProblemInstance const& _problem, parameters_t& _parameters){
    this->population.clear();
    this->evaluationCounter = 0;
    this->problemInstance = _problem;
    this->parameters = _parameters;
    std::vector<generationResult> allGenResults(this->parameters.generations);

    auto start = std::chrono::high_resolution_clock::now();

    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    initializePopulation();
    std::cout << "Population initialized!" << std::endl;
    int generation;
    for (generation = 0; generation < this->parameters.generations-1; generation++) {
        std::cerr << "Generation " << generation << " proceeding...      ";
        allGenResults[generation] = summarizePopulation(this->population[generation]);

        std::pair<individual_t, individual_t> parents = selectParents(this->population[generation]);
        std::pair<individual_t, individual_t> children;
        std::vector<individual_t> selectionPool = this->population[generation];

        if (dist(gen) < this->parameters.crossoverPropability) {
            children = crossover(parents.first.chromosome, parents.second.chromosome);

            if (dist(gen) < this->parameters.mutationPropability) {
                mutation(children.first);
            }
    
            if (dist(gen) < this->parameters.mutationPropability) {
                mutation(children.second);
            }

            selectionPool.push_back(children.first);
            selectionPool.push_back(children.second);
        }

        std::cout << "DONE!" << std::endl;

        this->population[generation+1] = selectGeneration(selectionPool);
    }
    allGenResults[generation] = summarizePopulation(this->population[generation]);

    std::cout << "All generations done!" << std::endl;

    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> time = stop - start;

    return {allGenResults, time, this->evaluationCounter};
}