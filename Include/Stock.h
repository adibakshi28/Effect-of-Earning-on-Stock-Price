#pragma once

#include <string>
#include <vector>
#include <iomanip>
#include "DataFetcher.h"
#include "DataDownloader.h"
#include "DataSorting.h"


namespace FinanceProject {

class Stock 
{
    private:
    string ticker;
    DataFetcher *df;
    // DataSorting *ds;
    // DataDownloader *dd;


    public:
    Stock():ticker(""),df(nullptr){};
    Stock(string ticker_, DataFetcher *df_):ticker(ticker_),df(df_){};
    ~Stock(){};
    void GetEveryInfo();
};


} 