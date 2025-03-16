#ifndef EVALUATE_H
#define EVALUATE_H

#include "instance.h"
#include <vector>

//finally not here
typedef struct individual_t{
    std::vector<Node*> chromosome;
    float fitnessValue = 0;
} individual_t;

float evaluateSolution(std::vector<Node*> const& solution, std::vector<std::vector<float>> const& distanceMatrix, const int& truckCapacity);

void evaluatePopulation(std::vector<individual_t>& population);

#endif