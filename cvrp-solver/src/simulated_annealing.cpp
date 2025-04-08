#include "../include/simulated_annealing.h"
#include "evaluate.h"
#include <cstdlib>
#include <random>
#include <ranges>
#include <algorithm>
#include <cmath>

sa_individual_t SimulatedAnnealing::generateRandomSolution() {
    std::vector<const Node*> randomRoute(this->problemInstance.nodes.size()-1);

    // go through all nodes except first, which is the depot
    int i=0;
    for (const Node& node : this->problemInstance.nodes | std::views::drop(1)) {
        randomRoute[i] = &node;
        i++;
    }

    std::shuffle(randomRoute.begin(), randomRoute.end(), this->gen);

    sa_individual_t randomSolution{randomRoute, evaluateSolution(randomRoute, this->problemInstance.distanceMatrix, this->problemInstance.getCapacity(), this->evaluationCounter)};

    return randomSolution;
}

sa_individual_t SimulatedAnnealing::generateNeighbor(sa_individual_t solution){ 
    std::vector<int> ints(solution.solution.size()-1);
    std::iota(ints.begin(), ints.end(), 0);
    std::shuffle(ints.begin(), ints.end(), this->gen);
    std::swap(solution.solution[ints[0]], solution.solution[ints[1]]);
    // for(int i=0; i<1; i+=2){
    //     std::swap(solution.solution[ints[i]], solution.solution[ints[i+1]]);
    // }
    solution.cost = evaluateSolution(solution.solution, this->problemInstance.distanceMatrix, this->problemInstance.getCapacity(), this->evaluationCounter);

    return solution;
}

sa_results_t SimulatedAnnealing::run(ProblemInstance const& _problem, sa_parameters_t& _parameters){
    this->problemInstance = _problem;
    this->parameters = _parameters;
    this->evaluationCounter = 0;
    this->temperature = this->parameters.initialTemp;
    this->solutions.clear();
    std::vector<float> allTemps;
    std::vector<float> allSolutions;

    auto start = std::chrono::high_resolution_clock::now();

    std::uniform_real_distribution<> dist(0.0, 1.0);
    this->solutions.push_back(generateRandomSolution());
    while (this->evaluationCounter < this->parameters.maxEvals) {
    // while (this->temperature > 0.0100f) {
        allTemps.push_back(this->temperature);
        allSolutions.push_back(this->solutions.back().cost);
        sa_individual_t currentSolution = generateNeighbor(this->solutions.back());
        if (currentSolution.cost < this->solutions.back().cost) {
            this->solutions.push_back(currentSolution);
        } else {
            float difference = std::abs(this->solutions.back().cost - currentSolution.cost);
            float propability = exp(-difference/this->temperature);
            if (dist(gen) < propability) {
                this->solutions.push_back(currentSolution);
            }
        }
        // this->temperature *= this->parameters.coolingRate;
        this->temperature = this->temperature / (1 + 0.0001 * this->temperature);
    }   

    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time = stop - start;

    return {allTemps, allSolutions, time, this->evaluationCounter};
}
