#pragma once

#include "instance.h"
#include <chrono>
#include <random>

typedef struct sa_parameters_t{
    int maxEvals;
    float initialTemp;
    float coolingRate;
} sa_parameters_t;

typedef struct sa_results_t{
    std::vector<float> allTemps;
    std::vector<float> allSolutions;
    std::chrono::duration<double> runTime;
    int numberOfEvaluateCalls;
} sa_results_t;

typedef struct sa_individual_t{
    std::vector<const Node*> solution;
    float cost = 0;
} sa_individual_t;

class SimulatedAnnealing {
    private:
        std::mt19937 gen;
        float temperature;
        sa_parameters_t parameters;
        ProblemInstance problemInstance;
        int evaluationCounter = 0;
        std::vector<sa_individual_t> solutions;

        sa_individual_t generateRandomSolution();
        sa_individual_t generateNeighbor(sa_individual_t previousSolution);
    public:
        sa_results_t run(ProblemInstance const& _problem, sa_parameters_t& _parameters);
        SimulatedAnnealing() : gen(std::random_device{}()) {};
        ~SimulatedAnnealing() {};
};