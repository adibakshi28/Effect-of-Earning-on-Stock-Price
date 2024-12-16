#include <vector>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include "Plotter.h"
using namespace std;

namespace FinanceProject
{
void Plotter::plot_caar()
{
    int i = 0;
    int N = df->GetN();
    int nIntervals = 2*N+1;
    int dataSize=nIntervals;
    
    vector<double> xdata;
    for(i=-N;i<=N;i++){
        xdata.push_back(i);
    }
    
    auto miss=bs->GetMetric()["caar_mean_miss"];
    auto meet=bs->GetMetric()["caar_mean_meet"];
    auto beat=bs->GetMetric()["caar_mean_beat"];

    FILE *gnuplotPipe;
    const char *tempDataFileName1 = "Miss";
    const char *tempDataFileName2 = "Meet";
    const char *tempDataFileName3 = "Beat";

    const char *title = "Avg CAAR for three groups";
    const char *yLabel = "Y";
    gnuplotPipe = popen("gnuplot -persist", "w"); //gnuplotPipe True/False

    fprintf(gnuplotPipe, "set grid\n");
    fprintf(gnuplotPipe, "set title '%s'\n", title);
    fprintf(gnuplotPipe, "set arrow from 0.5,graph(0,0) to 0.5,graph(1,1) nohead lc rgb 'red'\n");
    fprintf(gnuplotPipe, "set xlabel 'Announcement Date'\nset ylabel '%s'\n", yLabel);
    if (gnuplotPipe) {
        fprintf(gnuplotPipe,"plot \"%s\" with lines, \"%s\" with lines, \"%s\" with lines\n",tempDataFileName1,tempDataFileName2,tempDataFileName3);

        // cout<<"this is step 1"<<endl;
        FILE * tempDataFile = fopen(tempDataFileName1,"w");
        for (i=0; i < dataSize; i++){
            fprintf(tempDataFile,"%lf %lf\n",xdata[i],miss[i]);
            // cout<<"this is step 2 "<<x<<" "<<y<<endl;
        }
        fclose(tempDataFile);

        // cout<<"this is step 3"<<endl;

        tempDataFile = fopen(tempDataFileName2,"w");
        for (i=0; i < dataSize; i++) {
            fprintf(tempDataFile,"%lf %lf\n",xdata[i],meet[i]);
        }
        fclose(tempDataFile);

        tempDataFile = fopen(tempDataFileName3,"w");
        for (i=0; i < dataSize; i++) {
            fprintf(tempDataFile,"%lf %lf\n",xdata[i],beat[i]);
        }
        fclose(tempDataFile);

        

        printf("press enter to continue...\n");
        pclose(gnuplotPipe);
        getchar();
        remove(tempDataFileName1);
        remove(tempDataFileName2);
        remove(tempDataFileName3);
        fprintf(gnuplotPipe,"exit \n");
    }
    else{
        printf("gnuplot not found...\n");
    }

    return ;
}

}