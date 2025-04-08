#include "evaluate.h"
#include "include/genetic_algorithm.h"
#include "include/instance.h"
#include "include/simulated_annealing.h"
#include <cfloat>
#include <iostream>
#include <format>
#include <fstream>
#include <limits>
#include <random>
#include <algorithm>

void saveGAResultsToCSV(const ga_results_t& results, const std::string& filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Nie można otworzyć pliku: " << filename << std::endl;
        return;
    }

    file << "Generation,BestFitness,AverageFitness,WorstFitness\n";

    for (size_t i = 0; i < results.generationResults.size(); ++i) {
        const auto& gen = results.generationResults[i];
        file << i << ","
             << gen.bestFitness << ","
             << gen.averageFitness << ","
             << gen.worstFitness << "\n";
    }

    file << "\nRunTime (seconds)," << results.runTime.count() << "\n";
    file << "Evaluate Calls," << results.numberOfEvaluateCalls << "\n";

    file.close();
}

void saveSAResultsToCSV(const sa_results_t& results, const std::string& filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Nie można otworzyć pliku: " << filename << std::endl;
        return;
    }

    file << "Step,Temperature,Solution\n";

    size_t steps = std::min(results.allTemps.size(), results.allSolutions.size());
    for (size_t i = 0; i < steps; ++i) {
        file << i << "," 
             << results.allTemps[i] << "," 
             << results.allSolutions[i] << "\n";
    }

    file << "\nRunTime (seconds)," << results.runTime.count() << "\n";
    file << "Evaluate Calls," << results.numberOfEvaluateCalls << "\n";

    file.close();
}

std::vector<const Node*> greedyTraversal(const ProblemInstance& problem) {
    const auto& nodes = problem.nodes;
    const auto& dist = problem.distanceMatrix;

    std::vector<bool> visited(nodes.size(), false);
    std::vector<const Node*> visitOrder;

    int currentNode = 0;
    visited[currentNode] = true;
    // visitOrder.push_back(&nodes[currentNode]);

    for (size_t step = 1; step < nodes.size(); ++step) {
        float minDist = std::numeric_limits<float>::max();
        int nextNode = -1;

        for (size_t i = 1; i < nodes.size(); ++i) {
            if (!visited[i] && dist[currentNode][i] < minDist) {
                minDist = dist[currentNode][i];
                nextNode = static_cast<int>(i);
            }
        }

        if (nextNode == -1) break;

        visited[nextNode] = true;
        visitOrder.push_back(&nodes[nextNode]);
        currentNode = nextNode;
    }

    return visitOrder;
}


std::vector<const Node*> randomTraversal(const ProblemInstance& problem) {
    const auto& nodes = problem.nodes;

    int depotIndex = -1;
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (nodes[i].id == 1) {
            depotIndex = static_cast<int>(i);
            break;
        }
    }

    if (depotIndex == -1) {
        std::cerr << "Nie znaleziono magazynu (id == 1)\n";
        return {};
    }

    std::vector<const Node*> customers;
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (static_cast<int>(i) != depotIndex) {
            customers.push_back(&nodes[i]);
        }
    }

    static std::random_device rd;
    static std::mt19937 rng(rd());
    std::shuffle(customers.begin(), customers.end(), rng);

    return customers;
}

template <typename T>
double standardDeviation(const std::vector<T>& data, double mean) {
    double variance = 0.0;
    for (T val : data) {
        variance += (val - mean) * (val - mean);
    }
    variance /= data.size();
    return std::sqrt(variance);
}

int main(){
    std::vector<ProblemInstance> problemInstances = readAllProblemInstances("data/");

    ga_parameters_t config_ga{
        // .generations = 100,
        .maxEvals = 50000,
        .populationSize = 100,
        .tournamentSize = 9,
        .elite = 6,
        .crossoverPropability = 0.8f,
        .mutationPropability = 0.2f
    };

    ga_results_t results_ga;

    GeneticAlgorithm alg_ga;
    // results_ga = alg_ga.run(problemInstances.back(), config_ga);

    // std::cout << "Run time: " << results_ga.runTime << std::endl;
    // std::cout << "Evaluate calls: " << results_ga.numberOfEvaluateCalls << std::endl;

    // saveGAResultsToCSV(results_ga, "ga_results.txt");

    // int i=1;
    // for (const auto& generation : results_ga.generationResults) {
    //     std::cout << std::format("Generation {:d}:   Best: {:f}   Avg: {:f}   Worst: {:f}", i, generation.bestFitness, generation.averageFitness, generation.worstFitness) << std::endl;
    //     i++;
    // }

    sa_parameters_t config_sa{
        .maxEvals = 50000,
        .initialTemp = 1000000,
        .coolingRate = 0.9f
    };

    sa_results_t results_sa;

    SimulatedAnnealing alg_sa;
    // results_sa = alg_sa.run(problemInstances.back(), config_sa);

    
    float ga_best_avg = 0;
    
    for (const auto& instance : problemInstances) {
        //GA
        std::cerr << instance.getName() << std::endl;
        // std::cerr << "GA ";
        generationResult ga_avg_results{FLT_MAX, 0, 0};
        float ga_tmp = 0.0f;
        std::vector<float> ga_results;
        for (int i=0; i<10; i++) {
            // std::cerr << i+1 << " ";
            results_ga = alg_ga.run(instance, config_ga);
            ga_avg_results.bestFitness = std::min(results_ga.generationResults.back().bestFitness, ga_avg_results.bestFitness);
            ga_avg_results.worstFitness = std::max(results_ga.generationResults.back().worstFitness, ga_avg_results.worstFitness);
            ga_tmp += results_ga.generationResults.back().averageFitness;
            ga_results.push_back(results_ga.generationResults.back().averageFitness);
            if (i == 3) {
                saveGAResultsToCSV(results_ga, std::format("GA_{:s}.txt", instance.getName()));
            }
        }
        ga_avg_results.averageFitness = ga_tmp/10.0f;
        double ga_stddev = standardDeviation(ga_results, ga_avg_results.averageFitness);

        ga_best_avg += ga_avg_results.bestFitness;

        std::cout << std::endl << "GA: " << ga_avg_results.bestFitness << " | " << ga_avg_results.averageFitness << " | " << ga_avg_results.worstFitness << " | " << ga_stddev << std::endl; //" | " << results_ga.numberOfEvaluateCalls <<

        //SA
        // std::cerr << "SA ";

        float sa_best = FLT_MAX;
        float sa_worst = 0;
        float sa_avg = 0;
        std::vector<float> sa_results;

        for (int i=0; i<10; i++) {
            // std::cerr << i+1 << " ";
            results_sa = alg_sa.run(problemInstances.back(), config_sa);
            sa_best = std::min(results_sa.allSolutions.back(), sa_best);
            sa_worst = std::max(results_sa.allSolutions.back(), sa_worst);
            sa_avg += results_sa.allSolutions.back();
            sa_results.push_back(results_sa.allSolutions.back());
            if (i == 3) {
                saveSAResultsToCSV(results_sa, std::format("SA_{:s}.txt", instance.getName()));
            }
        }
        sa_avg /= 10.0f;
        double sa_stddev = standardDeviation(sa_results, sa_avg);

        std::cout << std::endl << "SA: " << sa_best << " | " << sa_avg << " | " << sa_worst << " | " << sa_stddev << std::endl; //" | " << results_sa.numberOfEvaluateCalls << 


        //RANDOM
        float avgRandom = 0.0f;
        float bestRandom = FLT_MAX;
        float worstRandom = 0.0f;
        std::vector<float> ra_results;
        for (int i=0; i<10; i++) {
            int tmp;
            auto randomOrder = randomTraversal(problemInstances.back());
            float cost = evaluateSolution(randomOrder, problemInstances.front().distanceMatrix, problemInstances.front().getCapacity(), tmp);
            bestRandom = std::min(cost, bestRandom);
            worstRandom = std::max(cost, worstRandom);
            avgRandom += cost;
            ra_results.push_back(cost);
        }
        avgRandom /= 10.0f;
        double ra_stddev = standardDeviation(ra_results, avgRandom);
        std::cout << "RANDOM: " << bestRandom << " | " << avgRandom << " | " << worstRandom << " | " << ra_stddev << std::endl;

        //GREEDY
        auto order = greedyTraversal(instance);
        int tmp;
        std::cout << "GREEDY: " << evaluateSolution(order, instance.distanceMatrix, instance.getCapacity(), tmp) << std::endl;
        
    }

    ga_best_avg /= (float)problemInstances.size();

    std::cout << "AVG:   " << ga_best_avg << std::endl;

    // saveSAResultsToCSV(results_sa, "sa_results.txt");

    // auto order = greedyTraversal(problemInstances.front());

    // std::cout << "Visit order:\n";
    // for (const Node* node : order) {
    //     std::cout << "Node ID: " << node->id << " | x: " << node->x << ", y: " << node->y << "\n";
    // }

    // int tmp = 0;
    // std::cout << "Length greedy: " << evaluateSolution(order, problemInstances.front().distanceMatrix, problemInstances.front().getCapacity(), tmp) << std::endl;

    // float avgRandom = 0.0f;
    // float bestRandom = FLT_MAX;
    // float worstRandom = 0.0f;
    // for (int i=0; i<10; i++) {
    //     auto randomOrder = randomTraversal(problemInstances.back());
    //     float cost = evaluateSolution(randomOrder, problemInstances.front().distanceMatrix, problemInstances.front().getCapacity(), tmp);
    //     bestRandom = std::min(cost, bestRandom);
    //     worstRandom = std::max(cost, worstRandom);
    //     avgRandom += cost;
    // }
    // avgRandom /= 10.0f;
    // std::cout << "Length random avg: " << avgRandom << std::endl;
    

    // std::cout << "Run time: " << results_sa.runTime << std::endl;
    // std::cout << "Evaluate calls: " << results_sa.numberOfEvaluateCalls << std::endl;

    // for (int i=0; i < results_sa.allSolutions.size(); i++){
    //     std::cout << std::format("Iteration: {:d}:   Sol: {:f}     Temp: {:f}", i+1, results_sa.allSolutions[i], results_sa.allTemps[i]) << std::endl;
    // }


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