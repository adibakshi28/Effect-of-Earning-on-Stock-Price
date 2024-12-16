#include <bits/stdc++.h>
#include "../Include/Bootstrapper.h"
#include "../Include/DataFetcher.h"
#include "../Include/DataSorting.h"
#include "../Include/DataDownloader.h"
using namespace std;

void Bootstrapper::PerformBootstrap(vector<string>& beatStocks, vector<string>& meetStocks, vector<string>& missStocks){
    // cout<<"Size of beat meet and miss stock groups -> ";
    // cout<<beatStocks.size()<<" "<<meetStocks.size()<<" "<<missStocks.size()<<" ";

    for(int i = 0;i < bootstrapIterations; i++){
        Sample beatSampler(beatStocks);
        vector<string> sampledBeat = beatSampler.get_sample(sampleSize); 
        beat.push_back(sampledBeat);

        Sample meetSampler(meetStocks);
        vector<string> sampledMeet = meetSampler.get_sample(sampleSize);
        meet.push_back(sampledMeet);
        
        Sample missSampler(missStocks);
        vector<string> sampledMiss = missSampler.get_sample(sampleSize); 
        miss.push_back(sampledMiss);
    }


    // DEBUG
    // for(int i = 0;i<bootstrapIterations;i++){
    //     cout<<"Group : "<<i+1<<endl;

    //     cout<<"Print Beat stocks group: ";
    //     for(size_t j = 0;j<beat[i].size();j++){
    //         cout<<beat[i][j]<<" ";
    //     }cout<<endl;

    //     cout<<"Print Meet stocks group: ";
    //     for(size_t j = 0;j<meet[i].size();j++){
    //         cout<<meet[i][j]<<" ";
    //     }cout<<endl;

    //     cout<<"Print Miss stocks group: ";
    //     for(size_t j = 0;j<miss[i].size();j++){
    //         cout<<miss[i][j]<<" ";
    //     }cout<<endl<<endl;
    // }

}

vector<string> Bootstrapper::AggregateBootstrappStocks(){
    unordered_set<string> allTickers;

    for(size_t i = 0;i<miss.size();i++){
        for(size_t j = 0;j<miss[i].size();j++){
            allTickers.insert(miss[i][j]);
        }
    }

    for(size_t i = 0;i<beat.size();i++){
        for(size_t j = 0;j<beat[i].size();j++){
            allTickers.insert(beat[i][j]);
        }
    }


    for(size_t i = 0;i<meet.size();i++){
        for(size_t j = 0;j<meet[i].size();j++){
            allTickers.insert(meet[i][j]);
        }
    }

    // DEBUG
    // cout<<"Number of aggregated stocks set is : "<<allTickers.size()<<endl;

    vector<string> allTickerVector;
    for(auto it = allTickers.begin();it!=allTickers.end();it++){
        allTickerVector.push_back(*it);
    }

    // DEBUG
    // cout<<"Number of aggregated stocks vector is : "<<allTickerVector.size()<<endl;

    return allTickerVector;
}



// Overload - for vector<double> to perform element wise subtraction 
vector<double> operator-(const vector<double>& lhs, const vector<double>& rhs) {
    if (lhs.size() != rhs.size()) {
        throw std::invalid_argument("Vectors must be the same size for subtraction.");
    }

    vector<double> result(lhs.size());
    for (size_t i = 0; i < lhs.size(); i++) {
        result[i] = lhs[i] - rhs[i];
    }
    return result;
}



void Bootstrapper::CalculateAllMetricsForAllGroups(DataDownloader downloader) {
    map<string, map<string, double>> stockDataMap = downloader.GetStockDataMap(); 

    map<string,double> iwvPrices = stockDataMap["IWV"];

    map<string, double> Rm;
    {
        auto iter_cur = iwvPrices.begin();
        auto iter_prev = iwvPrices.begin();
        if(iter_cur != iwvPrices.end()) iter_cur++;
        while(iter_cur != iwvPrices.end()) {
            double p_t = iter_cur->second;
            double p_t_minus = iter_prev->second;
            Rm[iter_cur->first] = log(p_t / p_t_minus);

            iter_cur++;
            iter_prev++;
        }
    }


    auto computeMeanStd = [&](const vector<vector<double>> &data) {
        int samples = (int)data.size();
        int length = (int)data[0].size();
        vector<double> mean(length, 0.0), stddev(length, 0.0);

        for (int i = 0; i < samples; i++) {
            for (int j = 0; j < length; j++) {
                mean[j] += data[i][j];
            }
        }
        for (int j = 0; j < length; j++) {
            mean[j] /= samples;
        }

        for (int i = 0; i < samples; i++) {
            vector<double> diffVec = data[i] - mean;  // element-wise subtraction
            for (int j = 0; j < length; j++) {
                stddev[j] += diffVec[j] * diffVec[j];
            }
        }
        for (int j = 0; j < length; j++) {
            stddev[j] = sqrt(stddev[j] / (samples - 1));
        }

        return make_pair(mean, stddev);
    };

    auto processCategory = [&](const vector<vector<string>> &category) {
        int totalDays = (int)stockDataMap[category[0][0]].size();
        int numReturns = totalDays - 1;

        vector<vector<double>> AAR_all_samples; 
        vector<vector<double>> CAAR_all_samples; 

        for (auto &sample : category) {
            vector<vector<double>> ARs;
            ARs.reserve(sample.size());

            for (auto &ticker : sample) {
                if (stockDataMap.find(ticker) == stockDataMap.end()) {
                    continue;
                }

                vector<string> eventDates;
                eventDates.reserve(stockDataMap[ticker].size());
                for (auto &p : stockDataMap[ticker]) {
                    eventDates.push_back(p.first);
                }
                sort(eventDates.begin(), eventDates.end());

                map<string,double> tickerPrices;
                for (auto &d : eventDates) {
                    tickerPrices[d] = stockDataMap[ticker][d];
                }

                map<string, double> Ri;
                {
                    auto iter_cur = tickerPrices.begin();
                    auto iter_prev = tickerPrices.begin();
                    if(iter_cur != tickerPrices.end()) iter_cur++;
                    while(iter_cur != tickerPrices.end()) {
                        double p_t = iter_cur->second;
                        double p_t_minus = iter_prev->second;
                        double ret = log(p_t / p_t_minus);
                        Ri[iter_cur->first] = ret;

                        iter_cur++;
                        iter_prev++;
                    }
                }


                vector<double> AR(numReturns,0.0);
                {
                    int ind = 0;
                    for (int i = 0; i < numReturns; i++) {
                        string retDate = eventDates[i+1]; 
                        if (Rm.find(retDate) == Rm.end()) {
                            cerr << "Warning: No Rm data for date: " << retDate << ". AR may be invalid." << endl;
                        }
                        AR[ind++] = Ri[retDate] - Rm[retDate];
                    }
                }

                ARs.push_back(AR);
            }

            vector<double> AAR(numReturns, 0.0);
            for (int i = 0; i < numReturns; i++) {
                double sum_ar = 0.0;
                int count = (int)ARs.size();
                if (count == 0) {
                    continue;
                }
                for (auto &ar : ARs) {
                    sum_ar += ar[i];
                }
                AAR[i] = sum_ar / count;
            }

            vector<double> CAAR(numReturns,0.0);
            CAAR[0] = AAR[0];
            for (int i = 1; i < numReturns; i++) {
                CAAR[i] = CAAR[i-1] + AAR[i];
            }

            AAR_all_samples.push_back(AAR);
            CAAR_all_samples.push_back(CAAR);
        }

        auto AAR_mean_std = computeMeanStd(AAR_all_samples);
        auto CAAR_mean_std = computeMeanStd(CAAR_all_samples);

        return make_tuple(AAR_mean_std.first, AAR_mean_std.second, CAAR_mean_std.first, CAAR_mean_std.second);
    };

    auto [aar_mean_miss, aar_std_miss, caar_mean_miss, caar_std_miss] = processCategory(miss);
    auto [aar_mean_beat, aar_std_beat, caar_mean_beat, caar_std_beat] = processCategory(beat);
    auto [aar_mean_meet, aar_std_meet, caar_mean_meet, caar_std_meet] = processCategory(meet);

    unordered_map<string, vector<double>> metrics_local;
    metrics_local["aar_mean_miss"] = aar_mean_miss;
    metrics_local["aar_mean_beat"] = aar_mean_beat;
    metrics_local["aar_mean_meet"] = aar_mean_meet;

    metrics_local["caar_mean_miss"] = caar_mean_miss;
    metrics_local["caar_mean_beat"] = caar_mean_beat;
    metrics_local["caar_mean_meet"] = caar_mean_meet;

    metrics_local["aar_std_miss"] = aar_std_miss;
    metrics_local["aar_std_beat"] = aar_std_beat;
    metrics_local["aar_std_meet"] = aar_std_meet;

    metrics_local["caar_std_miss"] = caar_std_miss;
    metrics_local["caar_std_beat"] = caar_std_beat;
    metrics_local["caar_std_meet"] = caar_std_meet;

    metrics = metrics_local;

// ___________________________________________________________________________________________

    vector<vector<vector<double>>> vector3D;

    vector3D.clear();
    vector3D.resize(3);
    for (int i = 0; i < 3; i++) {
        vector3D[i].resize(4);
    }

    // miss metrics
    vector3D[0][0] = aar_mean_miss;    // AAR_AVG
    vector3D[0][1] = aar_std_miss;     // AAR_STD
    vector3D[0][2] = caar_mean_miss;   // CAAR_AVG
    vector3D[0][3] = caar_std_miss;    // CAAR_STD

    // meet metrics
    vector3D[1][0] = aar_mean_meet;    
    vector3D[1][1] = aar_std_meet;     
    vector3D[1][2] = caar_mean_meet;   
    vector3D[1][3] = caar_std_meet;    

    // beat metrics
    vector3D[2][0] = aar_mean_beat;    
    vector3D[2][1] = aar_std_beat;     
    vector3D[2][2] = caar_mean_beat;  
    vector3D[2][3] = caar_std_beat;   

    metrics3D = vector3D;
}


// Function to print metrics for one group
unordered_map<string,vector<double>> Bootstrapper::ShowGroupMetrics(const unordered_map<string, vector<double>> &metrics, const string &groupName) {
    string aar_mean_key = "aar_mean_" + groupName;
    string aar_std_key = "aar_std_" + groupName;
    string caar_mean_key = "caar_mean_" + groupName;
    string caar_std_key = "caar_std_" + groupName;

    const vector<double> &aar_mean = metrics.at(aar_mean_key);
    const vector<double> &aar_std = metrics.at(aar_std_key);
    const vector<double> &caar_mean = metrics.at(caar_mean_key);
    const vector<double> &caar_std = metrics.at(caar_std_key);

    unordered_map<string, vector<double>> m;
    m["aar_mean"] = aar_mean;
    m["aar_std"] = aar_std;
    m["caar_mean"] = caar_mean;
    m["caar_std"] = caar_std;

    return m;
}


// Function to retrieve CAAR mean data for all three groups
unordered_map<string,vector<double>> Bootstrapper::GetAllGroupsCAAR(const unordered_map<string, vector<double>> &metrics) {
    const vector<double> &caar_mean_miss = metrics.at("caar_mean_miss");
    const vector<double> &caar_mean_beat = metrics.at("caar_mean_beat");
    const vector<double> &caar_mean_meet = metrics.at("caar_mean_meet");

    unordered_map<string, vector<double>> m;
    m["miss"] = caar_mean_miss;
    m["beat"] = caar_mean_beat;
    m["meet"] = caar_mean_meet;

    return m;
}