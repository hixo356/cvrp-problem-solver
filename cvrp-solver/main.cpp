#include "genetic_algorithm.h"
#include "include/instance.h"

int main(){
    std::vector<ProblemInstance> problemInstances = readAllProblemInstances("data/");

    GeneticAlgorithm alg;
    alg.initializePopulation();
}