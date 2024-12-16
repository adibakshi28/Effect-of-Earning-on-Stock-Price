#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <ctime>
#include <cstdlib>
#include "DataFetcher.h"

using namespace std;

class SortingGroup {
private:
    vector<string> beat;  
    vector<string> meet;  
    vector<string> miss; 
    vector<string> sorting_symbol;  
    map<string, string> symbol_group; 
    
    map<string, earningsData> earningsDataMap;

public:
    SortingGroup(map<string, earningsData> earningsMap)
        : earningsDataMap(earningsMap) {
            srand((unsigned)time(NULL));
        }

    void sort_group();

    vector<string> getBeat() const { return beat; }
    vector<string> getMeet() const { return meet; }
    vector<string> getMiss() const { return miss; }
    const map<string, string>& getSymbolGroup() const { return symbol_group; }
};

class Sample{
    private:
        vector<string> population;  
        const unsigned int DEFAULT_SAMPLE_SIZE = 30; 
    
    public:
        Sample(){}
        Sample(vector<string> const &population_):population(population_){}
        vector<string> get_sample(unsigned int M = 30);
        void SetPopulation(const vector<string> &population_){population = population_;}
        ~Sample(){}
};