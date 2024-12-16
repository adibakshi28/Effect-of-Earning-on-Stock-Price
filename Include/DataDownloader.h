#pragma once

#include <map>
#include <string>
#include <vector>
#include <curl/curl.h>
#include "DataFetcher.h"

using namespace std;

typedef map<string, double> Stock_data;

class DataDownloader {
private:
    map<string, Stock_data> stockDataMap; 

public:
    DataDownloader() {}
    ~DataDownloader() {}


    // Independent helper functions (static)
    static int write_data(void* ptr, int size, int nmemb, FILE* stream);
    static void* myrealloc(void* ptr, size_t size);
    static int write_data2(void* ptr, size_t size, size_t nmemb, void* data);

    // Methods to download data
    static Stock_data downloadData(CURL* handle, const string& symbol, const string& date, const string& start_date, const string& end_date);
    static void fetchDataFromEod(const vector<string>& symbolVector,
                                 map<string, Stock_data>& stockDataMap,
                                 unsigned N,
                                 const vector<string>& earningsDates,
                                 const map<string, string>& earningsDatesStartMap,
                                 const map<string, string>& earningsDatesEndMap
                                 );
    map<string, Stock_data>& GetStockDataMap() { return stockDataMap; }
};
