#include "DataSorting.h"
#include <algorithm>
#include <iostream>
#include <cstdlib>

void SortingGroup::sort_group() {
    beat.clear();
    meet.clear();
    miss.clear();

    sorting_symbol.clear();
    symbol_group.clear();
    
    vector<double> vector_surprise_percent; 
    string symbol;

    // DEBUG
    // cout<<"Size of stock data map : "<<earningsDataMap.size()<<endl;
    // for(auto it = earningsDataMap.begin();it!=earningsDataMap.end();it++){
    //     cout<<"Ticker is : "<<it->first<<endl;
    // }
    
    for (map<string, earningsData> ::const_iterator itr = earningsDataMap.begin(); 
         itr != earningsDataMap.end(); itr++) {
        
        if (itr->first == "IWV") continue;
        

        sorting_symbol.push_back(itr->first);
        vector_surprise_percent.push_back(itr->second.surprisePercent);
        
    }
    
    vector<double> sorting_surprise_percent = vector_surprise_percent;
    
    sort(sorting_surprise_percent.begin(), sorting_surprise_percent.end());
    
    int third = sorting_surprise_percent.size() / 3;
    double threshold1 = sorting_surprise_percent[third];        
    double threshold2 = sorting_surprise_percent[third * 2];    

    for (unsigned int i = 0; i < vector_surprise_percent.size(); i++) {
        symbol = sorting_symbol[i];
        double surp = vector_surprise_percent[i];
        
        if (surp < threshold1) {
            miss.push_back(symbol);
            symbol_group[symbol] = "miss";
        }
        else if (surp > threshold2) {
            beat.push_back(symbol);
            symbol_group[symbol] = "beat";
        }
        else {
            meet.push_back(symbol);
            symbol_group[symbol] = "meet";
        }
    }
}

vector<string> Sample::get_sample(unsigned int M) {
    vector<string> sample;
    unsigned N = population.size(); 
    
    if (N <= M) {
        return population;
    }
    
    set<int> s;

    while (s.size() < M) {
        int random_value = (rand() % N); 
        s.insert(random_value);
    }
    
    sample.reserve(M); 
    for (set<int>::iterator it = s.begin(); it != s.end(); ++it) {
        sample.push_back(population[*it]);
    }
    
    return sample;
}