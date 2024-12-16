#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>
#include "../Include/Menu.h"

using namespace std;

int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    try
    {
        Menu menu(0);
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
    }

    auto end = std::chrono::high_resolution_clock::now(); 

    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Runtime: " << duration.count() << " ms" << std::endl;

    return 0;
}
