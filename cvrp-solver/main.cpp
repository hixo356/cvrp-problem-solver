#include "include/genetic_algorithm.h"
#include "include/instance.h"
#include <iostream>
#include <format>

int main(){
    std::vector<ProblemInstance> problemInstances = readAllProblemInstances("data/");

    parameters_t config{
        // .generations = 100,
        .maxEvals = 2000,
        .populationSize = 200,
        .tournamentSize = 5,
        .crossoverPropability = 0.7f,
        .mutationPropability = 0.3f
    };

    results_t results;

    GeneticAlgorithm alg;
    results = alg.run(problemInstances.back(), config);

    std::cout << "Run time: " << results.runTime << std::endl;
    std::cout << "Evaluate calls: " << results.numberOfEvaluateCalls << std::endl;

    int i=1;
    for (const auto& generation : results.generationResults) {
        std::cout << std::format("Generation {:d}:   Best: {:f}   Avg: {:f}   Worst: {:f}", i, generation.bestFitness, generation.averageFitness, generation.worstFitness) << std::endl;
        i++;
    }


    // GeneticAlgorithm alg;
    // alg.problemInstance = problemInstances[0];
    // //alg.initializePopulation();

    // std::vector<const Node*> parent1(9);
    // std::vector<const Node*> parent2(9);
    // std::vector<Node> nodes;
    // for (int i=0; i<9; i++) {
    //     Node node = {i+1, 0,0,0};
    //     nodes.push_back(node);
    // }

    // parent1 = {&nodes[0], &nodes[1], &nodes[2], &nodes[3], 
    //             &nodes[4], &nodes[5], &nodes[6], &nodes[7], &nodes[8]};

    // parent2 = {&nodes[3], &nodes[2], &nodes[0], &nodes[1], 
    //     &nodes[7], &nodes[6], &nodes[4], &nodes[5], &nodes[8]};




    // std::pair<individual_t, individual_t> children = alg.crossover(parent1, parent2);

    // for (auto gene : children.first.chromosome) {
    //     std::cout<<gene->id<<" ";
    // }
    // std::cout<<std::endl;

    // for (auto gene : children.second.chromosome) {
    //     std::cout<<gene->id<<" ";
    // }
    // std::cout<<std::endl;

    // alg.mutation(children.first);
    // alg.mutation(children.second);

    // for (auto gene : children.first.chromosome) {
    //     std::cout<<gene->id<<" ";
    // }
    // std::cout<<std::endl;

    // for (auto gene : children.second.chromosome) {
    //     std::cout<<gene->id<<" ";
    // }
    // std::cout<<std::endl;
}