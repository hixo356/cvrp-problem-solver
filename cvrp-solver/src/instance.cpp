#include "../include/instance.h"

#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

void ProblemInstance::readInstanceFromFile(std::string filePath){
    std::ifstream inputFile("data/test.txt");
    std::string line;

    if (!inputFile.is_open()) {
        std::cerr << "Failed to open \"" << filePath << "\" file!" << std::endl;
        return;
    }

    // NAME
    std::getline(inputFile, line);
    std::istringstream ss(line);
    ss.ignore(6);
    ss >> this->name;
    
    // OPTIMAL VALUE
    std::getline(inputFile, line);
    std::regex pattern("Optimal value: (\\d+)");
    std::smatch match;
    std::regex_search(line, match, pattern);
    this->optimalResult = std::stoi(match[1].str());

    // SKIP LINE
    std::getline(inputFile, line);

    // DIMENSION
    std::getline(inputFile, line);
    ss.str(line);
    ss.clear();
    ss.ignore(11);
    ss >> this->dimension;

    // SKIP 3 LINES
    std::getline(inputFile, line);
    std::getline(inputFile, line);
    std::getline(inputFile, line);

    // READ NODES
    for (int i=0; i<this->dimension; i++) {
        struct Node node;

        inputFile >> node.id >> node.x >> node.y;
        this->nodes.push_back(node);
    }

    // SKIP LINE
    std::getline(inputFile, line);
    std::getline(inputFile, line);

    // READ DEMANDS
    for (int i=0; i<this->dimension; i++) {
        int tmp, tmp2;

        inputFile >> tmp >> tmp2;

        this->nodes[i].demand = tmp2;
    }

    inputFile.close();
}

void ProblemInstance::printInstanceData() const {
    std::cout << "Problem Name: " << name << std::endl;
    std::cout << "Optimal Value: " << optimalResult << std::endl;
    std::cout << "Dimension: " << dimension << std::endl;

    std::cout << "Nodes:" << std::endl;
    for (const auto& node : nodes) {
        std::cout << "Node ID: " << node.id
                  << ", X: " << node.x
                  << ", Y: " << node.y
                  << ", Demand: " << node.demand
                  << std::endl;
    }
}
