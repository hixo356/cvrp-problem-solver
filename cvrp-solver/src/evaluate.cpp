#include "../include/evaluate.h"
#include "../include/instance.h"

// node id= -1 is separator
float evaluateSolution(std::vector<const Node*> const& solution, std::vector<std::vector<float>> const& distanceMatrix, const int& truckCapacity, int& counter){
    counter++;

    float penalty = 0;
    float totalDistance = 0;
    int routeDemand = 0;
    int excees = 0;

    // int i = 0;
    // if (solution[0]->id == -1) {
    //     i = 1;
    // }
    // while (i < solution.size()) {
    //     // start from depot
    //     totalDistance += distanceMatrix[1][solution[i]->id];
    //     routeDemand += solution[i]->demand;
    //     i++;

    //     while (i < solution.size() && solution[i]->id != -1) {
    //         totalDistance += distanceMatrix[solution[i-1]->id][solution[i]->id];
    //         routeDemand += solution[i]->demand;
    //         i++;
    //     }
        
    //     // finish in depot
    //     totalDistance += distanceMatrix[solution[i-1]->id][1];

    //     if (routeDemand > truckCapacity) {
    //         int excess = routeDemand - truckCapacity;
    //         penalty += excees * excees;
    //     }

    //     routeDemand = 0;
    //     i++;
    // }

    int i = 0;
    int currentDemand = 0;
    while(i < solution.size()) {
        // start from depot
        totalDistance += distanceMatrix[1][solution[i]->id];
        currentDemand += solution[i]->demand;
        i++;

        while (i < solution.size()) {
            currentDemand += solution[i]->demand;
            if (currentDemand > truckCapacity) {
                break;
            }
            totalDistance += distanceMatrix[solution[i-1]->id][solution[i]->id];
            i++;
        }

        // do {
        //     totalDistance += distanceMatrix[solution[i-1]->id][solution[i]->id];
        //     currentDemand += solution[i]->demand;
        //     i++;
        // }while (currentDemand <= truckCapacity);
        
        totalDistance += distanceMatrix[solution[i-1]->id][1];
        currentDemand = 0;
    }
    
    // return fitness value
    // return 1 / (totalDistance + penalty); 
    return totalDistance;
}