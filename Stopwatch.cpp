#include "Stopwatch.h"

Stopwatch::Stopwatch()
{
    //ctor
    // Jan Potocki 2017-2019
}

void Stopwatch::start()
{
    tstart = std::chrono::steady_clock::now();
}

void Stopwatch::stop()
{
    tstop = std::chrono::steady_clock::now();
    measurement = tstop - tstart;
}

double Stopwatch::read()
{
    return measurement.count();
}
