#include "../include/evaluate.h"
#include "instance.h"

// node id= -1 is separator
float evaluateSolution(std::vector<Node*> const& solution, std::vector<std::vector<float>> const& distanceMatrix, const int& truckCapacity){

    float penalty = 0;
    float totalDistance = 0;
    int routeDemand = 0;
    int excees = 0;

    int i=0;
    while (i < solution.size()) {
        // start from depot
        totalDistance += distanceMatrix[1][solution[i]->id];
        routeDemand += solution[i]->demand;
        i++;

        while (i < solution.size() && solution[i]->id != -1) {
            totalDistance += distanceMatrix[solution[i-1]->id][solution[i]->id];
            routeDemand += solution[i]->demand;
            i++;
        }
        // finish in depot
        totalDistance += distanceMatrix[solution[i-1]->id][1];

        if (routeDemand > truckCapacity) {
            int excess = routeDemand - truckCapacity;
            penalty += excees * excees;
        }

        i++;
    }
    
    // return fitness value
    return 1 / (totalDistance + penalty); 
}

void evaluatePopulation(std::vector<individual>& population, std::vector<std::vector<float>> const& distanceMatrix, const int& truckCapacity){
    for (auto& individual : population) {
        individual.fitnessValue = evaluateSolution(individual.chromosome, distanceMatrix, truckCapacity);
    }
}