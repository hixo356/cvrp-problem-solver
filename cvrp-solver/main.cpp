#include "include/instance.h"

#include <iostream>

int main(){
    std::cout<<"Hello world!"<<std::endl;

    ProblemInstance test;
    test.readInstanceFromFile("../test.txt");
    test.printInstanceData();
}