#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <chrono>

// Klasa do pomiaru czasu (wieloplatformowa)
// Jan Potocki 2017-2019
class Stopwatch
{
    public:
        Stopwatch();
        void start();
        void stop();
        double read();
    protected:
    private:
        std::chrono::duration<double> measurement;
        std::chrono::time_point<std::chrono::steady_clock> tstart;
        std::chrono::time_point<std::chrono::steady_clock> tstop;
};

#endif // STOPWATCH_H
