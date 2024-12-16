#include <bits/stdc++.h>
#include <chrono>
#include "Menu.h"

#include "../Include/DataFetcher.h"
#include "../Include/DataDownloader.h"
#include "../Include/DataSorting.h"
#include "../Include/Bootstrapper.h"
#include "../Include/Plotter.h"
#include "../Include/Stock.h"


using namespace std;
using namespace FinanceProject;

Menu::Menu(int initChoice)
    : choice(initChoice),
      fetcher(),
      downloader(),  
      bootstrapper(40, 30) { 
    DisplayMenu();
    ProcessChoice();
}


void Menu::DisplayMenu() {
    cout << "\n----- Financial Computing Final Project Menu -----\n";
    cout << "1. Retrieve Historical Price Data\n";
    cout << "2. Pull Stock Information\n";
    cout << "3. Display AAR and CAAR Metrics\n";
    cout << "4. Show CAAR Graph\n";
    cout << "5. Exit\n";
    cout << "--------------------------------------------\n";
}

void Menu::ProcessChoice() {
    do {
        cout << "\nEnter your choice (1-5): ";
        cin >> choice;

        switch (choice) {
        case '1':
            fetcher.enterN();
            RetrieveHistoricalPriceData();
            break;
        case '2':
            PullStockInformation();
            break;
        case '3':
            DisplayAARAndCAARMetrics();
            break;
        case '4':
            ShowCAARGraph();
            break;
        case '5':
            cout << "Exiting the program. Goodbye!\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }

        if (choice != '5') {
            DisplayMenu();
        }

    } while (choice != '5');
}

void Menu::RetrieveHistoricalPriceData() {
    using namespace std::chrono;

    auto start_time = high_resolution_clock::now();

    cout << "[LOG] Sorting stocks into groups..." << endl<<endl;
    SortingGroup sorter(fetcher.GetEarningsDataMap());
    sorter.sort_group();

    vector<string> beatStocks = sorter.getBeat();
    vector<string> meetStocks = sorter.getMeet();
    vector<string> missStocks = sorter.getMiss();

    bootstrapper.PerformBootstrap(beatStocks, meetStocks, missStocks);

    auto allStocks = bootstrapper.AggregateBootstrappStocks();

    cout << "[LOG] Size of aggregated stock list: " << allStocks.size() << endl<<endl;

    int n = 3;
    int group_size = allStocks.size() / n;
    vector<vector<string>> stock_groups(n);

    for (int i = 0; i < n; ++i) {
        int start = i * group_size;
        int end = (i == n - 1) ? allStocks.size() : (i + 1) * group_size;
        stock_groups[i] = vector<string>(allStocks.begin() + start, allStocks.begin() + end);
    }

    vector<thread> threads;
    mutex mapMutex;

    auto& a = downloader.GetStockDataMap();
    int b = fetcher.GetN();
    auto c = fetcher.GetEarningsDates();
    auto d = fetcher.GetEarningsDatesStartMap();
    auto e = fetcher.GetEarningsDatesEndMap();

    thread thread_1([&]() { downloader.fetchDataFromEod(stock_groups[0], a, b, c, d, e); });
    thread thread_2([&]() { downloader.fetchDataFromEod(stock_groups[1], a, b, c, d, e); });
    thread thread_3([&]() { downloader.fetchDataFromEod(stock_groups[2], a, b, c, d, e); });

    thread_1.join();
    thread_2.join();
    thread_3.join();

    bootstrapper.CalculateAllMetricsForAllGroups(downloader);

    auto end_time = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(end_time - start_time).count();
    cout << "Time taken to execute RetrieveHistoricalPriceData: " << duration << " milliseconds." << endl;
}


void Menu::PullStockInformation() {
    
    string stock_symbol;
    cout << "Print the stock symbol: "<<endl;
    cin >> stock_symbol;
    Stock mystock(stock_symbol,&fetcher);
    mystock.GetEveryInfo();

}

ostream& operator<<(ostream& os, const vector<double>& vec) {
    const int valuesPerLine = 10;
    int count = 0;
    
    for (auto val : vec) {
        os << setw(8) << val << " ";
        count++;
        if (count % valuesPerLine == 0) os << "\n";
    }

    if (vec.size() % valuesPerLine != 0) {
        os << "\n";
    }

    return os;
}


void Menu::DisplayAARAndCAARMetrics() {
    cout << "Displaying AAR and CAAR metrics...\n";

    string groupName = "";
    while (groupName != "miss" && groupName != "meet" && groupName != "beat") {
        cout << "Enter group name (miss, meet, beat): ";
        cin >> groupName;
    }
    cout << endl << endl;

    unordered_map<string, vector<double>> metricsResult = bootstrapper.ShowGroupMetrics(bootstrapper.GetMetric(), groupName);

    cout << "Avg AARs for Group " << groupName << ":\n" << metricsResult["aar_mean"] << "\n";
    cout << "AAR STD for Group " << groupName << ":\n" << metricsResult["aar_std"] << "\n";
    cout << "Avg CAARs for Group " << groupName << ":\n" << metricsResult["caar_mean"] << "\n";
    cout << "CAAR STD for Group " << groupName << ":\n" << metricsResult["caar_std"] << "\n";
}



void Menu::ShowCAARGraph() {
    cout << "Showing CAAR graph...\n";

    // unordered_map<string, vector<double>> metricsResult;
    // auto metrics = bootstrapper.GetMetric();

    Plotter myplotter(&fetcher,&bootstrapper);
    myplotter.plot_caar();
}