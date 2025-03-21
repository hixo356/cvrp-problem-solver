#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

#include "instance.h"
#include <chrono>
#include <random>

typedef struct parameters_t{
    int generations;
    int populationSize;
    int tournamentSize;
    float crossoverPropability;
    float mutationPropability;
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
        std::mt19937 gen;
        struct Node* sepNode;
        std::vector<std::vector<individual_t>> population;
        int evaluationCounter = 0;
        ProblemInstance problemInstance;
        parameters_t parameters;

        void initializePopulation();
        void mutation(individual_t& individual);
        void evaluatePopulation(std::vector<individual_t>& population);
        generationResult summarizePopulation(std::vector<individual_t> const& population) const;
        std::vector<individual_t> selectGeneration(std::vector<individual_t> selectionPool);
        std::pair<individual_t, individual_t> selectParents(std::vector<individual_t> selectionPool);
        
    public:
        results_t run(ProblemInstance const& _problem, parameters_t& _parameters);
        std::pair<individual_t, individual_t> crossover(std::vector<const Node*> const& parent1, std::vector<const Node*> const& parent2);
        GeneticAlgorithm() : gen(std::random_device{}()), sepNode(new struct Node({-1, 0, 0, 0})) {};
        ~GeneticAlgorithm(){ delete sepNode; };
};

#endif