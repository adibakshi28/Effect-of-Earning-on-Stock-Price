#include "DataFetcher.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>

using namespace std;

void DataFetcher::enterN(){
    cout << "Please enter N to retrieve 2N+1 days of historical price data for all stocks.";
    cin >> N;

    while (N < 40 || N > 80) {
        cout << "N should be between 40 and 80, Please try again." << endl;
        cin >> N;
    }cout<<endl;

    componentStocksFile = "Data/iShares-Russell-3000-ETF_fund.csv";
    earningsAnnouncementsFile = "Data/Russell3000EarningsAnnouncements.csv";
    tradingCalendarFile = "Data/market_calendar.csv";

    // 1. Read all tickers
    readAndStoreTickers(allTickers);

    // 2. Read and store Earnings Data
    readAndStoreEarningsData(stockSymbols, earningsDates, allTickers);

    // 3. Retrieve trading dates
    retrieveTradingDates(tradingDates);

    // 4. Calculate start and end dates
    calculateStartAndEndDateForDataFetch(earningsDatesStartMap, earningsDatesEndMap);
}

void DataFetcher::readAndStoreTickers(vector<string> &allTickers) {
    ifstream fin(componentStocksFile);
    if (!fin) {
        cout << "Not able to read file: " << componentStocksFile << endl;
        return;
    }

    string line, ticker;
    // Skip header
    getline(fin, line);

    while (getline(fin, line)) {
        stringstream ss(line);
        getline(ss, ticker, ',');
        allTickers.push_back(ticker);
    }

    cout << "[LOG] Size of all tickers: " << allTickers.size() << endl<<endl;
    fin.close();
}

void DataFetcher::readAndStoreEarningsData(vector<string>& symbols, vector<string>& dates, const vector<string>& allTickers) {
    ifstream fin(earningsAnnouncementsFile);
    if (!fin) {
        cout << "Not able to read file: " << earningsAnnouncementsFile << endl;
        return;
    }

    string line, date, symbol, periodEnding;
    double estimate, reported, surprise, surprisePercent;

    getline(fin, line); // skip header
    while (getline(fin, line)) {
        stringstream ss(line);
        getline(ss, symbol, ',');
        getline(ss, date, ',');
        getline(ss, periodEnding, ',');
        ss >> estimate; ss.ignore();
        ss >> reported; ss.ignore();
        ss >> surprise; ss.ignore();
        ss >> surprisePercent;

        // Adjust date if it's weekend
        struct tm tm = {0};
        strptime(date.c_str(), "%Y-%m-%d", &tm);
        mktime(&tm);
        int wday = tm.tm_wday;
        if (wday == 0) tm.tm_mday += 1; // Sunday -> Monday
        else if (wday == 6) tm.tm_mday += 2; // Saturday -> Monday

        mktime(&tm);
        char buffer[11];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tm);
        date = string(buffer);

        // Additional adjustment for 2024-09-02
        if (date == "2024-09-02") {
            tm.tm_mday += 1; // Move one day forward
            mktime(&tm);
            strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tm);
            date = string(buffer);
        }

        if (find(allTickers.begin(), allTickers.end(), symbol) != allTickers.end()) {
            earningsData ed = {date, periodEnding, estimate, reported, surprise, surprisePercent};
            earningsDataMap[symbol] = ed;
            symbols.push_back(symbol);
            dates.push_back(date);
        }
    }
    fin.close();
}


void DataFetcher::retrieveTradingDates(vector<string>& dates) {
    ifstream fin(tradingCalendarFile);
    if (!fin) {
        cout << "Not able to read file: " << tradingCalendarFile << endl;
        return;
    }

    string line;
    while (getline(fin, line)) {
        dates.push_back(line.substr(0, 10));
    }
    fin.close();
}

void DataFetcher::calculateStartAndEndDateForDataFetch(map<string, string>& startDateMap, map<string, string>& endDateMap) {
    for (const auto& eaDate : earningsDates) {
        auto it = find(tradingDates.begin(), tradingDates.end(), eaDate);
        if (it != tradingDates.end()) {
            int index = (int)(it - tradingDates.begin());
            int startIndex = index - (int)N - 1;
            if (startIndex >= 0) {
                startDateMap[eaDate] = tradingDates[startIndex];
            }

            unsigned endIndex = index + N;
            if (endIndex < tradingDates.size()) {
                endDateMap[eaDate] = tradingDates[endIndex];
            }
        }
    }
}
