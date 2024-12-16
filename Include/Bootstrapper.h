#pragma once

#include <vector>
#include <map>
#include <bits/stdc++.h>
#include "DataFetcher.h"
#include "DataDownloader.h"

using namespace std;

class Bootstrapper {
private:
    int bootstrapIterations;
    int sampleSize;
    vector<vector<string>> miss, beat, meet;
    unordered_map<string, vector<double>> metrics;         
    // aar_mean_miss, aar_mean_beat, aar_mean_meet, caar_mean_miss, caar_mean_beat, caar_mean_meet, aar_std_miss, aar_std_beat, aar_std_meet, caar_std_miss, caar_std_beat, caar_std->meet
    vector<vector<vector<double>>> metrics3D;

public:
    Bootstrapper(int iterations = 40, int size = 30){
        bootstrapIterations = iterations;
        sampleSize = size;
    }
    // ~Bootstrapper();

    void PerformBootstrap(vector<string>& beatStocks, vector<string>& meetStocks, vector<string>& missStocks);
    vector<string> AggregateBootstrappStocks();
    void CalculateAllMetricsForAllGroups(DataDownloader fetcher);
    unordered_map<string,vector<double>> ShowGroupMetrics(const unordered_map<string, vector<double>> &metrics, const string &groupName);
    unordered_map<string,vector<double>> GetAllGroupsCAAR(const unordered_map<string, vector<double>> &metrics);

    // Getter Methods
    int GetIterations(){
        return bootstrapIterations;
    }
    int GetSampleSize(){
        return sampleSize;
    }
    vector<vector<string>> GetMissGroup(){
        return miss;
    }
    vector<vector<string>> GetBeatGroup(){
        return beat;
    }
    vector<vector<string>> GetMeetGroup(){
        return meet;
    }
    unordered_map<string, vector<double>> GetMetric(){
        return metrics;
    }
};


