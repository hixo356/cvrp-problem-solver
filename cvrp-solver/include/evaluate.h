#ifndef EVALUATE_H
#define EVALUATE_H

#include "instance.h"
#include <vector>

//finally not here
typedef struct individual{
    std::vector<Node*> chromosome; //preferably pointers to nodes or sth
    float fitnessValue = 0;
} individual;

float evaluateSolution(std::vector<Node*> const& solution, std::vector<std::vector<float>> const& distanceMatrix, const int& truckCapacity);

void evaluatePopulation(std::vector<individual>& population);

#endif