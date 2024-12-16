#pragma once

#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <iomanip>
#include "DataFetcher.h"
#include "Bootstrapper.h"

namespace FinanceProject {

class Plotter{
    private:
    DataFetcher *df;
    Bootstrapper *bs;

    public:
    Plotter():df(nullptr),bs(nullptr){}
    Plotter(DataFetcher *df_,  Bootstrapper *bs_):df(df_),bs(bs_){}
    void plot_caar();
};

}