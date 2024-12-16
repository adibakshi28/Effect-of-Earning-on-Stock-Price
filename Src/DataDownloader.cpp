#include "DataDownloader.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <map>
#include <stdexcept>
#include <curl/curl.h>
#include <thread>
#include <chrono>

using namespace std;

struct MemoryStruct {
    vector<char> memory;

    void append(const char* data, size_t size) {
        memory.insert(memory.end(), data, data + size);
    }

    const char* data() const {
        return memory.data();
    }

    size_t size() const {
        return memory.size();
    }

    void clear() {
        memory.clear();
    }
};

int DataDownloader::write_data(void* ptr, int size, int nmemb, FILE* stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return (int)written;
}

int DataDownloader::write_data2(void* ptr, size_t size, size_t nmemb, void* data) {
    size_t realsize = size * nmemb;
    MemoryStruct* mem = static_cast<MemoryStruct*>(data);
    mem->append(static_cast<const char*>(ptr), realsize);
    return (int)realsize;
}


Stock_data DataDownloader::downloadData(CURL* handle, const string& symbol, const string& date, const string& start_date, const string& end_date) {
    Stock_data stockPrices;
    string url_common = "https://eodhistoricaldata.com/api/eod/";
    string api_token = "67474cf3ba4e10.64365179";

    MemoryStruct data;

    string url_request;
    if (end_date.empty()) {
        url_request = url_common + symbol + ".US?" + "from=" + start_date + "&api_token=" + api_token + "&period=d";
    } else {
        url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
    }

    while (true) { // Retry loop
        curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data);

        CURLcode status = curl_easy_perform(handle);
        if (status != CURLE_OK) {
            // cerr << "curl_easy_perform() failed: " << curl_easy_strerror(status) << endl;
            return stockPrices;
        }

        long http_code = 0;
        curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code == 200) {
            break; // Success, exit the retry loop
        } else if (http_code == 429) {
            // cerr << "Rate limit hit for " << symbol << ". Switching API token and retrying..." << endl;
            api_token = " 674aa75f92f8f6.25095014"; // Switch API token
            url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Small delay before retry
        } else {
            cerr << "HTTP error " << http_code << " for " << symbol << endl;
            return stockPrices;
        }
    }

    istringstream sData(string(data.data(), data.size()));
    string line;

    while (getline(sData, line)) {
        size_t found = line.find('-');
        if (found != string::npos) {
            string sDate = line.substr(0, line.find_first_of(','));
            stringstream ss(line);
            string token;
            vector<string> tokens;
            double dValue;

            while (getline(ss, token, ',')) {
                tokens.push_back(token);
            }

            try {
                dValue = strtod(tokens[5].c_str(), nullptr);
            } catch (const exception& e) {
                cerr << "Error parsing value: " << e.what() << endl;
                continue;
            }

            stockPrices[sDate] = dValue;
        }
    }

    if (stockPrices.empty()) {
        cerr << "No valid data for " << symbol << endl;
    }

    // cout << "Size of stock price " << stockPrices.size() << endl;


    return stockPrices;
}


void DataDownloader::fetchDataFromEod(
    const vector<string>& symbolVector,
    map<string, Stock_data>& stockDataMap,
    unsigned N,
    const vector<string>& earningsDates,
    const map<string, string>& earningsDatesStartMap,
    const map<string, string>& earningsDatesEndMap
) {
    CURL* handle = curl_easy_init();
    if (!handle) {
        cerr << "Curl init failed!" << endl;
        return;
    }

    // Common curl options
    curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0");
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);

    string symbol = "IWV";
    string start_date = "2024-01-01";
    string end_date;
    string date;
    
    Stock_data stockData = downloadData(handle, symbol, date, start_date, end_date);
    stockDataMap[symbol] = stockData;

    // Loading bar parameters
    int totalSymbols = (int)symbolVector.size();
    int barWidth = 50; // width of the loading bar in characters

    for (size_t i = 0; i < symbolVector.size(); i++) {
        // Calculate and print the loading bar
        double progress = (double)i / (double)totalSymbols;
        int pos = (int)(barWidth * progress);

        std::cout << "\rDownloading data: [";
        for (int j = 0; j < barWidth; ++j) {
            if (j < pos) std::cout << "=";
            else if (j == pos) std::cout << ">";
            else std::cout << " ";
        }

        std::cout << "] " << int(progress * 100.0) << "% (" 
                  << i*3 << "/" << totalSymbols*3 << ")";
        std::cout.flush();

        const string& symbol = symbolVector[i];
        const string& date = earningsDates[i];

        auto start_it = earningsDatesStartMap.find(date);
        auto end_it = earningsDatesEndMap.find(date);

        if (start_it == earningsDatesStartMap.end() || end_it == earningsDatesEndMap.end()) {
            // Could not find start or end date for this symbol
            continue;
        }

        string start_date = start_it->second;
        string end_date = end_it->second;

        Stock_data stockData = downloadData(handle, symbol, date, start_date, end_date);

        if (stockData.size() == (2 * N + 2)) {
            stockDataMap[symbol] = stockData;
        } 
    }

    std::cout << "\rDownloaded data: [";
    for (int j = 0; j < barWidth; ++j) {
        std::cout << "=";
    }
    std::cout << "] 100% (" << totalSymbols << "/" << totalSymbols << ")\n";

    curl_easy_cleanup(handle);
}
