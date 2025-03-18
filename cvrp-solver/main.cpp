#include "genetic_algorithm.h"
#include "include/instance.h"
#include <iostream>

int main(){
    std::vector<ProblemInstance> problemInstances = readAllProblemInstances("data/");

    GeneticAlgorithm alg;
    alg.problemInstance = problemInstances[0];
    //alg.initializePopulation();

    std::vector<const Node*> parent1(9);
    std::vector<const Node*> parent2(9);
    std::vector<Node> nodes;
    for (int i=0; i<9; i++) {
        Node node = {i+1, 0,0,0};
        nodes.push_back(node);
    }

    parent1 = {&nodes[0], &nodes[1], &nodes[2], &nodes[3], 
                &nodes[4], &nodes[5], &nodes[6], &nodes[7], &nodes[8]};

    parent2 = {&nodes[3], &nodes[2], &nodes[0], &nodes[1], 
        &nodes[7], &nodes[6], &nodes[4], &nodes[5], &nodes[8]};




    std::pair<individual_t, individual_t> children = alg.crossover(parent1, parent2);

    for (auto gene : children.first.chromosome) {
        std::cout<<gene->id<<" ";
    }
    std::cout<<std::endl;

    for (auto gene : children.second.chromosome) {
        std::cout<<gene->id<<" ";
    }
    std::cout<<std::endl;
}