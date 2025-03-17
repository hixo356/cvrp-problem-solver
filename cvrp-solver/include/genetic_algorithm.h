#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

#include "instance.h"
#include <chrono>

typedef struct parameters_t{
    float populationSize;
    float crossoverPropability;
    float mutationPropability;
    int tournamentSize;
    int generations;
} parameters_t;

struct generationResult{
    float bestFitness;
    float averageFitness;
    float worstFitness;
};

typedef struct results_t{
    std::vector<struct generationResult> generationResults;
    std::chrono::duration<double> runTime;
    int numberOfEvaluateCalls;
} results_t;

typedef struct individual_t{
    std::vector<const Node*> chromosome;
    float fitnessValue = 0;
} individual_t;

class GeneticAlgorithm{
    private:
        parameters_t parameters;
        struct Node* sepNode;
        std::vector<std::vector<individual_t>> population;
        const ProblemInstance problemInstance;
        
        std::vector<individual_t> selectGeneration(std::vector<individual_t> selectionPool);
        std::pair<individual_t, individual_t> selectParents(std::vector<individual_t> selectionPool);
        std::pair<individual_t, individual_t> crossover(individual_t const& parent1, individual_t const& parent2);
        individual_t mutation(individual_t const& individual);

        void evaluatePopulation(std::vector<individual_t>& population, std::vector<std::vector<float>> const& distanceMatrix, const int& truckCapacity);
    public:
        results_t run(ProblemInstance const& problem, parameters_t& parameters);
        void initializePopulation();

        GeneticAlgorithm(){ sepNode = new struct Node({-1, 0, 0, 0}); }
        ~GeneticAlgorithm(){ delete sepNode; };
};

#endif