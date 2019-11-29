#include "Graph.h"
#include "Stopwatch.h"
#include <algorithm>
#include <chrono>
#include <queue>
#include <random>
#include <thread>

#include <iostream>

Graph::Graph()
{
    //ctor
}

Graph::~Graph()
{
    //dtor
}

unsigned Graph::getVertexNumber()
{
    return vertexNumber;
}

void Graph::randomGenerateFullGraph(Graph &graph, unsigned maxWeight)
{
    std::random_device randomSrc;
    std::default_random_engine randomGen(randomSrc());
    std::uniform_int_distribution<> weightDist(1, maxWeight);

    for(int i = 0; i < graph.vertexNumber; i++)
    {
        for(int j = 0; j < graph.vertexNumber; j++)
        {
            if(i != j)
            {
                // Bez warunku na krawedzie juz wygenerowane...
                // ...z tym radzi sobie juz metoda addEdge
                int randomWeight = weightDist(randomGen);
                graph.addEdge(i, j, randomWeight);
            }
        }
    }
}

std::vector<unsigned> Graph::travellingSalesmanBruteForce(Graph &graph)
{
    // ALGORYTM przegladu zupelnego
    // Implementacja: Jan Potocki 2017
    // (refactoring 2019)
    std::vector<unsigned> vertexArray;

    // Generowanie "spisu" wierzcholkow
    // (od razu w odpowiedniej kolejnosci dla next_permutation)
    for(int i = 1; i < graph.vertexNumber; i++)
        vertexArray.push_back(i);

    std::vector<unsigned> minCombination;
    int minRoute = -1;

    // Petla przegladajaca kolejne permutacje
    do
    {
        std::vector<unsigned> combination;

        // Dodanie wierzcholka startowego i pierwszego na trasie
        combination.push_back(0);
        combination.push_back(vertexArray.front());

        // W petli reszta wiercholkow
        for(int i = 1; i < vertexArray.size(); i++)
            combination.push_back(vertexArray.at(i));

        // Powrot do wierzcholka startowego
        combination.push_back(0);
        // PEA 2
        // Jan Potocki 2017

        int route = 0;
        for(int i = 1; i < combination.size(); i++)
            route += graph.getWeight(combination.at(i - 1), combination.at(i));

        if(minRoute == -1 || route < minRoute)
        {
            minRoute = route;
            minCombination = combination;
        }
    }
    while(next_permutation(vertexArray.begin(), vertexArray.end()));

    return minCombination;
}

std::vector<unsigned> Graph::travellingSalesmanBranchAndBound(Graph &graph)
{
    // ALGORYTM pracujacy w oparciu o kolejke priorytetowa i niejawnie utworzone drzewo
    // Zrodlo: www.ii.uni.wroc.pl/~prz/2011lato/ah/opracowania/met_podz_ogr.opr.pdf
    // Autor: Mateusz Lyczek 2011
    // Implementacja: Jan Potocki 2017
    std::priority_queue<std::vector<unsigned>, std::vector< std::vector<unsigned> >, RouteComparison> routeQueue;
    std::vector<unsigned> optimalRoute;     // Tu bedziemy zapisywac optymalne (w danej chwili) rozwiazanie
    int optimalRouteLength = -1;            // -1 - bedziemy odtad uznawac, ze to jest nieskonczonosc ;-)

    // UMOWA
    // Pierwszy element wektora to dlugosc trasy (trzeba ustawic "z palca"!)
    // Kolejne to wierzcholki na trasie
    std::vector<unsigned> currentRoute;     // Niejawne tworzenie drzewa, tu bedzie korzen
    currentRoute.push_back(0);              // Poczatkowe oszacowanie nie ma znaczenia
    currentRoute.push_back(0);              // Wierzcholek startowy (korzen drzewa rozwiazan)
    routeQueue.push(currentRoute);          // Dodanie do kolejki korzenia

    while(!routeQueue.empty())
    {
        // Przypisanie korzenia do dalszej roboty
        currentRoute = routeQueue.top();
        routeQueue.pop();

        // Sprawdzenie, czy rozwiazanie jest warte rozwijania, czy odrzucic
        if(optimalRouteLength == -1 || currentRoute.at(0) < optimalRouteLength)
        {
            for(int i = 0; i < graph.vertexNumber; i++)
            {
                // Petla wykonywana dla kazdego potomka rozpatrywanego wlasnie rozwiazania w drzewie
                // Ustalenie, czy dany wierzcholek mozna jeszcze wykorzystac, czy juz zostal uzyty
                bool vertexUsed = false;
                for(int j = 1; j < currentRoute.size(); j++)
                {
                    if(currentRoute.at(j) == i)
                    {
                        vertexUsed = true;
                        break;
                    }
                }
                if(vertexUsed)
                    continue;

                // Niejawne utworzenie nowego wezla reprezuntujacego rozpatrywane rozwiazanie...
                std::vector<unsigned> nextRoute = currentRoute;
                //unsigned nextLength = graph.getWeight(nextRoute.back(), i);
                nextRoute.push_back(i);

                // Dalej bedziemy postepowac roznie...
                if(nextRoute.size() > graph.vertexNumber)
                {
                    // Doszlismy wlasnie do liscia
                    // Dodajemy droge powrotna, nie musimy nic szacowac
                    // (wszystko juz wiemy)
                    nextRoute.push_back(0);

                    nextRoute.at(0) = 0;

                    for(int j = 1; j < nextRoute.size() - 1; j++)
                    {
                        // Liczymy dystans od poczatku do konca
                        nextRoute.at(0) += graph.getWeight(nextRoute.at(j), nextRoute.at(j+ 1));
                    }
                    if(optimalRouteLength == -1 || nextRoute.at(0) < optimalRouteLength)
                    {
                        optimalRouteLength = nextRoute.at(0);
                        nextRoute.erase(nextRoute.begin());
                        optimalRoute = nextRoute;
                    }
                }
                else
                {
                    // Liczenie tego, co juz wiemy, od nowa...
                    // (dystans od poczatku)
                    nextRoute.at(0) = 0;
                    for(int j = 1; j < nextRoute.size() - 1; j++)
                    {
                        nextRoute.at(0) += graph.getWeight(nextRoute.at(j), nextRoute.at(j + 1));
                    }

                    // Reszte szacujemy...
                    // Pomijamy od razu wierzcholek startowy
                    for(int j = 1; j < graph.vertexNumber; j++)
                    {
                        // Odrzucenie wierzcholkow juz umieszczonych na trasie
                        bool vertexUsed = false;
                        for(int k = 1; k < currentRoute.size(); k++)
                        {
                            if(j == currentRoute.at(k))
                            {
                                vertexUsed = true;
                                break;
                            }
                        }
                        if(vertexUsed)
                            continue;

                        int minEdge = -1;
                        for(int k = 0; k < graph.vertexNumber; k++)
                        {
                            // Odrzucenie krawedzi do wierzcholka 0 przy ostatnim wierzcholku w czesciowym rozwiazaniu
                            // Wyjatkiem jest ostatnia mozliwa krawedz
                            if(j == i && k == 0)
                                continue;

                            // Odrzucenie krawedzi do wierzcholka umieszczonego juz na rozwazanej trasie
                            bool vertexUsed = false;
                            for(int l = 2; l < nextRoute.size(); l++)
                            {
                                if(k == nextRoute.at(l))
                                {
                                    vertexUsed = true;
                                    break;
                                }
                            }
                            if(vertexUsed)
                                continue;

                            // Odrzucenie samego siebie
                            if(k == j)
                                continue;

                            // Znalezienie najkrotszej mozliwej jeszcze do uzycia krawedzi
                            unsigned consideredLength = graph.getWeight(j, k);

                            if(minEdge == -1)
                                minEdge = consideredLength;
                            else if(minEdge > consideredLength)
                                minEdge = consideredLength;
                        }
                        nextRoute.at(0) += minEdge;
                    }

                    // ...i teraz zastanawiamy sie co dalej
                    if(optimalRouteLength == -1 || nextRoute.at(0) < optimalRouteLength)
                    {
                        routeQueue.push(nextRoute);
                    }
                }
            }
        }
        else
        {
            // Jezeli jedno rozwiazanie odrzucilismy, to wszystkie inne tez mozemy
            // (kolejka priorytetowa, inne nie moga byc lepsze)
            break;
        }
    }

    return optimalRoute;
}

std::vector<unsigned> Graph::travellingSalesmanGreedy(Graph &graph, unsigned startVertex)
{
    // ALGORYTM zachlanny z wierzcholkiem startowym przekazanym w parametrze
    // Implementacja: Jan Potocki 2017
    std::vector<unsigned> route;

    // Przypisanie wierzcholka startowego
    route.push_back(startVertex);

    for(int i = 0; i < graph.vertexNumber - 1; i++)
    {
        int minEdge = -1;
        unsigned nextVertex;
        for(int j = 0; j < graph.vertexNumber; j++)
        {
            // Odrzucenie samego siebie lub wierzcholka startowego
            // (zeby bylo szybciej)
            if(route.back() == j || route.front() == j)
                continue;

            // Odrzucenie krawedzi do wierzcholka umieszczonego juz na trasie
            bool vertexUsed = false;
            for(int k = 0; k < route.size(); k++)
            {
                if(j == route.at(k))
                {
                    vertexUsed = true;
                    break;
                }
            }
            if(vertexUsed)
                continue;

            // Znalezienie najkrotszej mozliwej jeszcze do uzycia krawedzi
            unsigned consideredLength = graph.getWeight(route.back(), j);

            if(minEdge == -1)
            {
                minEdge = consideredLength;
                nextVertex = j;
            }
            else if(minEdge > consideredLength)
            {
                minEdge = consideredLength;
                nextVertex = j;
            }
        }
        route.push_back(nextVertex);
    }

    route.push_back(startVertex);
    return route;
}

std::vector<unsigned> Graph::travellingSalesmanHybrid(Graph &graph)
{
    // ALGORYTM hybrydowy losowo-zachlanny
    // Losowa czesc wierzcholkow jest losowana, reszta zachlannie
    // Implementacja: Jan Potocki 2019
    std::vector<unsigned> route;

    std::random_device randomSrc;
    std::default_random_engine randomGen(randomSrc());
    std::uniform_int_distribution<> vertexNumberDist(1, graph.vertexNumber);
    std::uniform_int_distribution<> vertexDist(0, graph.vertexNumber - 1);

    // Liczba losowanych wierzcholkow
    unsigned randomVertexNumber = vertexNumberDist(randomGen);

    // Czesc losowa
    for(int i = 0; i < randomVertexNumber; i++)
    {
        unsigned randomVertex;
        bool vertexUsed;

        do
        {
            randomVertex = vertexDist(randomGen);
            vertexUsed = false;

            for(int j = 0; j < route.size(); j++)
            {
                if(route.at(j) == randomVertex)
                {
                    vertexUsed = true;
                    break;
                }
            }
        } while(vertexUsed == true);

        route.push_back(randomVertex);
    }

    // Czesc zachlanna
    for(int i = 0; i < graph.vertexNumber - randomVertexNumber; i++)
    {
        int minEdge = -1;
        unsigned nextVertex;
        for(int j = 0; j < graph.vertexNumber; j++)
        {
            // Odrzucenie samego siebie lub wierzcholka startowego
            // (zeby bylo szybciej)
            if(route.back() == j || route.front() == j)
                continue;

            // Odrzucenie krawedzi do wierzcholka umieszczonego juz na trasie
            bool vertexUsed = false;
            for(int k = 0; k < route.size(); k++)
            {
                if(j == route.at(k))
                {
                    vertexUsed = true;
                    break;
                }
            }
            if(vertexUsed)
                continue;

            // Znalezienie najkrotszej mozliwej jeszcze do uzycia krawedzi
            unsigned consideredLength = graph.getWeight(route.back(), j);

            // PEA 2 Plus
            // Jan Potocki 2019
            if(minEdge == -1)
            {
                minEdge = consideredLength;
                nextVertex = j;
            }
            else if(minEdge > consideredLength)
            {
                minEdge = consideredLength;
                nextVertex = j;
            }
        }
        route.push_back(nextVertex);
    }

    route.push_back(route.front());
    return route;
}

std::vector<unsigned> Graph::travellingSalesmanRandom(Graph &graph)
{
    // ALGORYTM losowy
    // Implementacja: Jan Potocki 2019
    std::vector<unsigned> route;

    std::random_device randomSrc;
    std::default_random_engine randomGen(randomSrc());
    std::uniform_int_distribution<> vertexDist(0, graph.vertexNumber - 1);

    for(int i = 0; i < graph.vertexNumber; i++)
    {
        unsigned randomVertex;
        bool vertexUsed;

        do
        {
            randomVertex = vertexDist(randomGen);
            vertexUsed = false;

            for(int j = 0; j < route.size(); j++)
            {
                if(route.at(j) == randomVertex)
                {
                    vertexUsed = true;
                    break;
                }
            }
        } while(vertexUsed == true);

        route.push_back(randomVertex);
    }

    route.push_back(route.front());
    return route;
}

std::vector<unsigned> Graph::travellingSalesmanTabuSearch(Graph &graph, unsigned tabuSteps, bool diversification, int iterationsToRestart, unsigned minStopTime, unsigned threadsNumber)
{
    // ALGORYTM wielawotkowy oparty na metaheurystyce tabu search
    // Pomocniczy kod uruchamiajacy watki wlasciwego algorytmu w najbardziej optymalny sposob
    // Implementacja: Jan Potocki 2019
    std::vector<unsigned> startVertexVector;
    std::vector<std::thread> threadsVector;
    std::vector<std::vector<unsigned>> resultsVector(threadsNumber);

    std::vector<int> resultsLength(threadsNumber);
    std::vector<unsigned> optimalResult;
    int optimalResultIndex;
    int optimalResultLength;

    std::random_device randomSrc;
    std::default_random_engine randomGen(randomSrc());
    std::uniform_int_distribution<> vertexDist(0, graph.vertexNumber - 1);

    // Petla uruchamiajaca watki
    for(int i = 0; i < threadsNumber; i++)
    {
        // Generowanie startowego rozwiazania...
        std::vector<unsigned> startRoute;
        unsigned startVertex;
        bool startVertexUsed;

        if(i < graph.vertexNumber)
        {
            // ...dopoki ma to sens - algorytmem zachlannym z innym wierzcholkiem startowym
            // (dla kazdego watku)
            do
            {
                startVertex = vertexDist(randomGen);
                startVertexUsed = false;

                for(int j = 0; j < startVertexVector.size(); j++)
                {
                    if(startVertexVector.at(j) == startVertex)
                    {
                        startVertexUsed = true;
                        break;
                    }
                }
            } while(startVertexUsed == true);

            // PEA 2 Plus
            // Jan Potocki 2019
            startVertexVector.push_back(startVertex);
            startRoute = Graph::travellingSalesmanGreedy(graph, startVertex);
        }
        else
        {
            // ...jezeli wszystkie wierzcholki sa juz wykorzystane - w pelni losowo
            startRoute = Graph::travellingSalesmanRandom(graph);
        }

        // Uruchomienie watku
        threadsVector.push_back(std::thread(Graph::travellingSalesmanTabuSearchEngine, std::ref(graph), tabuSteps, diversification, iterationsToRestart, minStopTime, startRoute, std::ref(resultsVector.at(i)), std::ref(resultsLength.at(i))));
    }

    // Petla potwierdzajaca zakonczenie watkow
    for(int i = 0; i < threadsNumber; i++)
        threadsVector.at(i).join();

    // Przegladanie wszystkich rozwiazan i wybor optymalnego
    optimalResultIndex = 0;
    optimalResultLength = resultsLength.at(0);

    for(int i = 0; i < threadsNumber; i++)
    {
        if(resultsLength.at(i) < optimalResultLength)
        {
            optimalResultIndex = i;
            optimalResultLength = resultsLength.at(i);
        }
    }

    optimalResult = resultsVector.at(optimalResultIndex);

    return optimalResult;
}

void Graph::travellingSalesmanTabuSearchEngine(Graph &graph, unsigned tabuSteps, bool diversification, int iterationsToRestart, unsigned minStopTime, std::vector<unsigned> startRoute, std::vector<unsigned> &result, int &resultLength)
{
    // ALGORYTM oparty na metaheurystyce tabu search z dywersyfikacja i sasiedztwem typu swap
    // Rdzen przeznaczony do uruchamiania jako jeden watek
    // Projekt i implementacja: Jan Potocki 2017
    // (refactoring 2019)
    Stopwatch onboardClock;

    std::vector<unsigned> optimalRoute;     // Tu bedziemy zapisywac optymalne (w danej chwili) rozwiazanie
    int optimalRouteLength = -1;            // -1 - bedziemy odtad uznawac, ze to jest nieskonczonosc ;-)
    std::vector<unsigned> currentRoute;     // Rozpatrywane rozwiazanie

    // Wyznaczenie poczatkowego rozwiazania algorytmem zachlannym
    //currentRoute = Graph::travellingSalesmanGreedy(graph);
    currentRoute = startRoute;

    // Inicjalizacja glownej petli...
    std::vector< std::vector<unsigned> > tabuArray;
    unsigned currentTabuSteps = tabuSteps;
    int stopCounter = 0;
    bool timeNotExceeded = true;
    onboardClock.start();

    // Rdzen algorytmu
    while(timeNotExceeded == true)
    {
        bool cheeseSupplied = true;
        bool intensification = false;

        while(cheeseSupplied == true)
        {
            std::vector<unsigned> nextRoute;
            int nextRouteLength = -1;

            std::vector<unsigned> nextTabu(3, 0);
            nextTabu.at(0) = currentTabuSteps;

            // Generowanie sasiedztwa typu swap przez zamiane wierzcholkow
            // (wierzcholka startowego i zarazem ostatniego nie ruszamy,
            // pomijamy tez od razu aktualny wierzcholek)
            for(int i = 1; i < graph.vertexNumber - 1; i++)
            {
                for(int j = i + 1; j < graph.vertexNumber; j++)
                {
                    std::vector<unsigned> neighbourRoute = currentRoute;

                    // Zamiana
                    unsigned buffer = neighbourRoute.at(j);
                    neighbourRoute.at(j) = neighbourRoute.at(i);
                    neighbourRoute.at(i) = buffer;

                    unsigned neighbourRouteLength = 0;
                    for(int i = 1; i < neighbourRoute.size(); i++)
                        neighbourRouteLength += graph.getWeight(neighbourRoute.at(i - 1), neighbourRoute.at(i));

                    // Sprawdzenie, czy dany ruch nie jest na liscie tabu
                    // (dwa wierzcholki)
                    bool tabu = false;
                    for(int k = 0; k < tabuArray.size(); k++)
                    {
                        if(tabuArray.at(k).at(1) == i && tabuArray.at(k).at(2) == j)
                        {
                            tabu = true;
                            break;
                        }

                        if(tabuArray.at(k).at(1) == j && tabuArray.at(k).at(2) == i)
                        {
                            tabu = true;
                            break;
                        }
                    }

                    // Kryterium aspiracji...
                    if(tabu == true && neighbourRouteLength >= optimalRouteLength)
                        // ...jezeli niespelnione - pomijamy ruch
                        continue;

                    if(nextRouteLength == -1)
                    {
                        nextRouteLength = neighbourRouteLength;
                        nextRoute = neighbourRoute;
                        nextTabu.at(1) = i;
                        nextTabu.at(2) = j;
                    }
                    else if(nextRouteLength > neighbourRouteLength)
                    {
                        nextRouteLength = neighbourRouteLength;
                        nextRoute = neighbourRoute;
                        nextTabu.at(1) = i;
                        nextTabu.at(2) = j;
                    }
                }
            }

            currentRoute = nextRoute;
            // PEA 2 Plus
            // Jan Potocki 2019

            if(optimalRouteLength == -1)
            {
                optimalRouteLength = nextRouteLength;
                optimalRoute = nextRoute;

                // Reset licznika
                stopCounter = 0;
            }
            else if(optimalRouteLength > nextRouteLength)
            {
                optimalRouteLength = nextRouteLength;
                optimalRoute = nextRoute;

                // Zaplanowanie intensyfikacji przy znalezieniu nowego optimum
                intensification = true;

                // Reset licznika
                stopCounter = 0;
            }

            // Weryfikacja listy tabu...
            int tabuPos = 0;
            while(tabuPos < tabuArray.size())
            {   
                // ...aktualizacja kadencji na liscie tabu
                tabuArray.at(tabuPos).at(0)--;
                
                //...usuniecie zerowych kadencji
                if(tabuArray.at(tabuPos).at(0) == 0)
                    tabuArray.erase(tabuArray.begin() + tabuPos);
                else
                    tabuPos++;
            }

            // ...dopisanie ostatniego ruchu do listy tabu
            tabuArray.push_back(nextTabu);

            // Zliczenie iteracji
            stopCounter++;

            // Zmierzenie czasu
            onboardClock.stop();
            if(onboardClock.read() > minStopTime)
                timeNotExceeded = false;

            // Sprawdzenie warunku zatrzymania
            if(diversification == true)
            {
                // Przy aktywowanej dywersyfikacji - po zadanej liczbie iteracji bez poprawy
                if(stopCounter >= iterationsToRestart || timeNotExceeded == false)
                    cheeseSupplied = false;
            }
            else
            {
                // Przy nieaktywowanej dywersyfikacji - po uplynieciu okreslonego czasu
                if(timeNotExceeded == false)
                    cheeseSupplied = false;
            }
        }

        // Dywersyfikacja
        if(diversification == true)
        {
            if(intensification == true)
            {
                // Intensyfikacja przeszukiwania przez skrócenie kadencji
                // (jezeli w ostatnim przebiegu znaleziono nowe minimum)
                currentRoute = optimalRoute;
                currentTabuSteps = tabuSteps / 4;
                intensification = false;
                // PEA 2 Plus
                // Jan Potocki 2019
            }
            else
            {
                // W innym przypadku wlasciwa dywersyfikacja przez wygenerowanie nowego
                // rozwiazania startowego algorytmem hybrydowym losowo-zachlannym
                currentRoute = Graph::travellingSalesmanHybrid(graph);
                currentTabuSteps = tabuSteps;
                intensification = false;
            }
        }

        // Reset licznika iteracji przed restartem
        stopCounter = 0;
    }

    result = optimalRoute;
    resultLength = optimalRouteLength;
}
