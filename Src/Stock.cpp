#include "Stock.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
namespace FinanceProject {

std::ostream& operator<<(std::ostream &os, const map<string,double> &st) 
{
    std::vector<std::string> dates;
    std::vector<double> prices;

    for (auto &kv : st) {
        dates.push_back(kv.first);
        prices.push_back(kv.second);
    }

    int columns = 11;
    int width = 14;

    {
        int i = 0;
        for (auto &d : dates) {
            os << std::setw(width) << d;
            i++;
            if (i % columns == 0) os << "\n";
        }
        if (!dates.empty() && dates.size() % columns != 0) os << "\n";
    }

    cout<<"===================================="<<endl;
    {
        int i = 0;
        for (auto &p : prices) {
            os << std::setw(width) << std::fixed << std::setprecision(3) << p;
            i++;
            if (i % columns == 0) os << "\n";
        }
        if (!prices.empty() && prices.size() % columns != 0) os << "\n";
    }

    return os;
}

void Stock::GetEveryInfo()
{   
    auto EarningsMap = df->GetEarningsDataMap();
    auto itr = EarningsMap.find(ticker);
    if (itr == EarningsMap.end()) {
        cout << "Not found!" << endl;
        return;
    }

    cout << "found" << endl;
    // Earning Announcement Date, Period Ending, Estimated, Reported Earnings, Surprise and Surprise %.
    cout << "====================================" << endl;
    cout << itr->first << ":" << endl;
    cout << "Period Ending: " << itr->second.date << endl;
    cout << "estimate: " << itr->second.estimate << endl;
    cout << "reported: " << itr->second.reported << endl;
    cout << "surprise: " << itr->second.surprise << endl;
    cout << "surprisePercent: " << itr->second.surprisePercent << endl;

    cout << "====================================" << endl;

    SortingGroup sorter_local(EarningsMap);
    sorter_local.sort_group();

    vector<string> beatStocks_local = sorter_local.getBeat();
    vector<string> meetStocks_local = sorter_local.getMeet();
    vector<string> missStocks_local = sorter_local.getMiss();

    auto itr_2 = find(beatStocks_local.begin(), beatStocks_local.end(), ticker);
    if (itr_2 != beatStocks_local.end()) {
        cout << "This stock belongs to beat group" << endl;
    } else {
        if (find(meetStocks_local.begin(), meetStocks_local.end(), ticker) != meetStocks_local.end()) {
            cout << "This stock belongs to meet group" << endl;
        } else if (find(missStocks_local.begin(), missStocks_local.end(), ticker) != missStocks_local.end()) {
            cout << "This stock belongs to miss group" << endl;
        }
    }

    cout << "====================================" << endl;

    cout << "Fetching the data:" << endl;
    vector<string> s1;
    s1.push_back(ticker);
    cout << "stock symbol: " << s1[0] << endl;
    DataDownloader mydd;
    mydd.fetchDataFromEod(s1, mydd.GetStockDataMap(), df->GetN(),
                          df->GetEarningsDates(), df->GetEarningsDatesStartMap(), df->GetEarningsDatesEndMap());

    map<string, double>::iterator itr_5, itr_6;
    auto stock_map = mydd.GetStockDataMap()[ticker];

    double price_0 = stock_map.begin()->second;
    map<string, double> cum_return, price;

    cout << "stock map size: " << stock_map.size() << endl;

    cout << "=================Daily Price===================" << endl;

    for (itr_5 = stock_map.begin(); itr_5 != stock_map.end(); itr_5++) {
        if (itr_5 != stock_map.begin()) {
            cum_return[itr_5->first] = log((itr_5->second) / price_0);
        }
        price[itr_5->first] = itr_5->second;
    }
    cout << price << endl;

    cout << "=================Cumulative Daily Log Return===================" << endl;
    cout << cum_return << endl;

    return;
}

}