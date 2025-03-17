#ifndef EVALUATE_H
#define EVALUATE_H

#include "instance.h"
#include <vector>

float evaluateSolution(std::vector<const Node*> const& solution, std::vector<std::vector<float>> const& distanceMatrix, const int& truckCapacity);

#endif