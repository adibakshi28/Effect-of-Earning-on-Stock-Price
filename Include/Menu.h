// Menu.h
#include <bits/stdc++.h>
#include "DataFetcher.h"
#include "DataDownloader.h"
#include "Bootstrapper.h"
using namespace std;


class Menu {
    char choice;
    DataFetcher fetcher;
    DataDownloader downloader;
    Bootstrapper bootstrapper;

public:
    Menu(int initChoice);

    void DisplayMenu();

    void ProcessChoice();

private:
    void RetrieveHistoricalPriceData();
    void PullStockInformation();
    void DisplayAARAndCAARMetrics();
    void ShowCAARGraph();
};
