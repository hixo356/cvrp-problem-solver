#ifndef INSTANCE_H
#define INSTANCE_H

#include <string>
#include <vector>

struct Node{
    int id = 0;
    int x = 0;
    int y = 0;
    int demand = 0;
};

class ProblemInstance{
    private:
        std::string name;
        
        int capacity = 0;
        int optimalResult = 0;
        int dimension = 0;
    public:
        // ProblemInstance(std::string _name, std::vector<Node> _nodes, int _capacity, int _optimalResult);
        ProblemInstance(){};
        std::vector<Node> nodes;
    
        int& getCapacity(){ return capacity; };
        int& getOptimalResult(){ return optimalResult; };
        std::vector<Node>& getNodes(){ return nodes; };
        std::string& getName(){ return name; };

        void readInstanceFromFile(std::string filePath);
        void printInstanceData() const;
        

};

#endif