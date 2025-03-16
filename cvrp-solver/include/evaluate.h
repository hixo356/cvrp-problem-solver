#ifndef EVALUATE_H
#define EVALUATE_H

#include "genetic_algorithm.h"
#include "instance.h"
#include <vector>

float evaluateSolution(std::vector<Node*> const& solution, std::vector<std::vector<float>> const& distanceMatrix, const int& truckCapacity);

void evaluatePopulation(std::vector<individual_t>& population);

#endif