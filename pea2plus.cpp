#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "Stopwatch.h"
#include "ArrayGraph.h"
#include "ListGraph.h"

using namespace std;

// USTAWIENIA
// Liczba powtorzen automatycznych pomiarow do usrednienia
const int measureIterations = 10;
// liczba automatycznych pomiarow
const int measureNumber = 4;
// Czas zatrzymania algorytmu tabu search w kazdym z automatycznych pomiarow
const int measureTabuStop[4] = {1, 5, 10, 15};
// Maksymalna odleglosc miast przy automatycznym generowaniu
const int measureSalesmanDistance = 400;

// Kadencja tabu search
const int tabuLength = 30;
// Kryterium dywersyfikacji, liczba iteracji bez poprawy
const int tabuIterationsToRestart = 10000;

// Wykorzystanie reprezentacji grafu w postaci list sasiedztwa...
// ...zamiast (domyslnie) macierzy sasiedztwa
// (wolniejsze obliczenia, mniejsze uzycie pamieci)
const bool useListGraph = false;

// Liczba watkow tabu search
const unsigned tabuThreadsNumber = 2;

// Domyslny czas zatrzymania algorytmu tabu search [s]
int tabuStopTime = 60;

// Domyslny stan dywersyfikacji
bool tabuDiversification = true;

int main()
{
    Stopwatch clock;                // czasomierz
    Graph *graph = NULL;            // <- tu bedziemy zapisywac adresy przez caly program

    cout << "PEA Projekt 2 v2.0ALPHA Plus" << endl;
    cout << "Jan Potocki 2017-2019" << endl;
    cout << "(beerware)" << endl;
    if(useListGraph)
        cout << "Uzycie listowej reprezentacji grafu" << endl;
    else
        cout << "Uzycie macierzowej reprezentacji grafu" << endl;
    cout << endl;

    int salesmanSelection;
    do
    {
        cout << "1 - wygeneruj losowe dane" << endl;
        cout << "2 - wyswietl dane" << endl;
        cout << "3 - dywersyfikacja TS" << endl;
        cout << "4 - czas zatrzymania TS" << endl;
        cout << "5 - tabu search" << endl;
        cout << "6 - algorytm zachlanny" << endl;
        cout << "7 - podzial i ograniczenia" << endl;
        cout << "8 - przeglad zupelny" << endl;
        cout << "9 - automatyczne pomiary (tabu search)" << endl;
        cout << "10 - wczytaj dane z pliku ATSP" << endl;
        cout << "11 - wczytaj dane z pliku TSP" << endl;
        cout << "Aby zakonczyc - 0" << endl;
        cout << "Wybierz: ";
        cin >> salesmanSelection;
        cout << endl;

        switch(salesmanSelection)
        {
            case 1:
            {
                int vertex;
                cout << "Liczba miast: ";
                cin >> vertex;
                cout << endl;

                if(graph != NULL)
                    delete graph;

                if(useListGraph)
                    graph = new ListGraph(vertex);
                else
                    graph = new ArrayGraph(vertex);

                Graph::randomGenerateFullGraph(*graph, measureSalesmanDistance);
            }
            break;
            case 2:
            {
                if(graph != NULL)
                    graph->displayGraph();
                else
                    cout << "Brak wygenerowanych danych" << endl;
                cout << endl;
            }
            break;
            case 3:
            {
                tabuDiversification = !tabuDiversification;

                if(tabuDiversification == true)
                    cout << "Dywersyfikacja TS zostala wlaczona" << endl;
                else
                    cout << "Dywersyfikacja TS zostala wylaczona" << endl;
                cout << endl;
            }
            break;
            case 4:
            {
                cout << "Poprzedni czas pracy TS: " << tabuStopTime << endl;
                cout << "Podaj nowy czas: ";
                cin >> tabuStopTime;
                cout << endl;
            }
            break;
            case 5:
            {
                if(graph != NULL)
                {
                    if(tabuStopTime != 0)
                    {
                        cout << "Kadencja: " << tabuLength << endl;
                        cout << "Czas zatrzymania algorytmu [s]: " << tabuStopTime << endl;

                        if(tabuDiversification == true)
                            cout << "Dywersyfikacja wlaczona, kryterium: " << tabuIterationsToRestart << " iteracji" << endl;
                        else
                            cout << "Dywersyfikacja wylaczona" << endl;

                        cout << endl;

                        clock.start();
                        vector<unsigned> route = Graph::travellingSalesmanTabuSearch(*graph, tabuLength, tabuDiversification, tabuIterationsToRestart, tabuStopTime, tabuThreadsNumber);
                        clock.stop();

                        // Wyswietlenie trasy
                        unsigned distFromStart = 0;
                        unsigned length = 0;
                        cout << route.at(0) << '\t' << length << '\t' << distFromStart << endl;
                        for(int i = 1; i < route.size(); i++)
                        {
                            length = graph->getWeight(route.at(i - 1), route.at(i));
                            distFromStart += length;

                            cout << route.at(i) << '\t' << length << '\t' << distFromStart << endl;
                        }

                        cout << "Dlugosc trasy: " << distFromStart << endl;
                        cout << endl;
                        cout << "Czas wykonania algorytmu [s]: " << clock.read() << endl;
                    }
                    else
                    {
                        // Easter egg ;-)
                        cout << "+++ MELON MELON MELON +++ Blad: Brak Sera! +++ !!!!! +++" << endl;
                    }
                }
                else
                    cout << "+++ MELON MELON MELON +++ Brak zaladowanych danych +++" << endl;
                cout << endl;
            }
            break;
            case 6:
            {
                if(graph != NULL)
                {
                    clock.start();
                    vector<unsigned> route = Graph::travellingSalesmanGreedy(*graph, 0);
                    clock.stop();

                    // Wyswietlenie trasy
                    unsigned distFromStart = 0;
                    unsigned length = 0;
                    cout << route.at(0) << '\t' << length << '\t' << distFromStart << endl;
                    for(int i = 1; i < route.size(); i++)
                    {
                        length = graph->getWeight(route.at(i - 1), route.at(i));
                        distFromStart += length;

                        cout << route.at(i) << '\t' << length << '\t' << distFromStart << endl;
                    }

                    cout << "Dlugosc trasy: " << distFromStart << endl;
                    cout << endl;
                    cout << "Czas wykonania algorytmu [s]: " << clock.read() << endl;
                }
                else
                    cout << "+++ MELON MELON MELON +++ Brak zaladowanych danych +++" << endl;
                cout << endl;
            }
            break;
            case 7:
            {
                if(graph != NULL)
                {
                    clock.start();
                    vector<unsigned> route = Graph::travellingSalesmanBranchAndBound(*graph);
                    clock.stop();

                    // Wyswietlenie trasy
                    unsigned distFromStart = 0;
                    unsigned length = 0;
                    cout << route.at(0) << '\t' << length << '\t' << distFromStart << endl;
                    for(int i = 1; i < route.size(); i++)
                    {
                        length = graph->getWeight(route.at(i - 1), route.at(i));
                        distFromStart += length;

                        cout << route.at(i) << '\t' << length << '\t' << distFromStart << endl;
                    }

                    cout << "Dlugosc trasy: " << distFromStart << endl;
                    cout << endl;
                    cout << "Czas wykonania algorytmu [s]: " << clock.read() << endl;
                }
                else
                    cout << "+++ MELON MELON MELON +++ Brak zaladowanych danych +++" << endl;
                cout << endl;
            }
            break;
            case 8:
            {
                if(graph != NULL)
                {
                    clock.start();
                    vector<unsigned> route = Graph::travellingSalesmanBruteForce(*graph);
                    clock.stop();

                    // Wyswietlenie trasy
                    unsigned distFromStart = 0;
                    unsigned length = 0;
                    cout << route.at(0) << '\t' << length << '\t' << distFromStart << endl;
                    for(int i = 1; i < route.size(); i++)
                    {
                        length = graph->getWeight(route.at(i - 1), route.at(i));
                        distFromStart += length;

                        cout << route.at(i) << '\t' << length << '\t' << distFromStart << endl;
                    }

                    cout << "Dlugosc trasy: " << distFromStart << endl;
                    cout << endl;
                    cout << "Czas wykonania algorytmu [s]: " << clock.read() << endl;
                }
                else
                    cout << "+++ MELON MELON MELON +++ Brak zaladowanych danych +++" << endl;
                cout << endl;
            }
            break;
            case 9:
            {
                // PEA 2
                // Jan Potocki 2017
                if(graph != NULL)
                {
                    double measureResults[measureNumber], measureResultsDiv[measureNumber];
                    for(int i = 0; i < measureNumber; i++)
                    {
                        measureResults[i] = 0;
                        measureResultsDiv[i] = 0;
                    }

                    cout << "Pomiary dla problemu komiwojazera, tabu search" << tabuLength << endl;
                    cout << "Kadencja: " << tabuLength << endl;
                    cout << "Kryterium dywersyfikacji: " << tabuIterationsToRestart << " iteracji" << endl;

                    // Petla pomiarowa
                    for(int krok = 0; krok < measureIterations; krok++)
                    {
                        for(int i = 0; i < measureNumber; i++)
                        {
                            vector<unsigned> route;
                            unsigned routeLength;

                            // Bez dywersyfikacji
                            cout << "Pomiar " << measureTabuStop[i] << " [s] (" << krok + 1 << " z " << measureIterations << " bez dywersyfikacji)..." << endl;

                            route = Graph::travellingSalesmanTabuSearch(*graph, tabuLength, false, tabuIterationsToRestart, measureTabuStop[i], tabuThreadsNumber);

                            routeLength = 0;
                            for(int j = 1; j < route.size(); j++)
                                routeLength += graph->getWeight(route.at(j - 1), route.at(j));
                            measureResults[i] += routeLength;

                            // Z dywersyfikacja
                            cout << "Pomiar " << measureTabuStop[i] << " [s] (" << krok + 1 << " z " << measureIterations << " z dywersyfikacja)..." << endl;

                            route = Graph::travellingSalesmanTabuSearch(*graph, tabuLength, true, tabuIterationsToRestart, measureTabuStop[i], tabuThreadsNumber);

                            routeLength = 0;
                            for(int j = 1; j < route.size(); j++)
                                routeLength += graph->getWeight(route.at(j - 1), route.at(j));
                            measureResultsDiv[i] += routeLength;
                        }
                    }

                    cout << "Opracowywanie wynikow..." << endl;

                    for(int i = 0; i < measureNumber; i++)
                    {
                        measureResults[i] = nearbyint(measureResults[i] / measureIterations);
                        measureResultsDiv[i] = nearbyint(measureResultsDiv[i] / measureIterations);
                    }

                    cout << "Zapis wynikow..." << endl;

                    ofstream salesmanToFile;
                    salesmanToFile.open("wyniki-komiwojazer-ts.txt");
                    salesmanToFile << "czas - bez dywersyfikacji - z dywersyfikacja" << endl;
                    for(int i = 0; i < measureNumber; i++)
                    {
                        salesmanToFile << measureTabuStop[i] << " [s]: " << (int)measureResults[i] << ' ' << (int)measureResultsDiv[i] << endl;
                    }
                    salesmanToFile.close();

                    cout << "Gotowe!" << endl;
                    cout << endl;
                }
                else
                {
                    cout << "+++ MELON MELON MELON +++ Brak zaladowanych danych +++" << endl;
                    cout << endl;
                }
            }
            break;
            case 10:
            {
                // Jan Potocki 2017
                string filename, fileInput;
                ifstream salesmanDataFile;

                cout << "Podaj nazwe pliku: ";
                cin >> filename;

                salesmanDataFile.open(filename.c_str());
                if(salesmanDataFile.is_open())
                {
                    do
                        salesmanDataFile >> fileInput;
                    while(fileInput != "DIMENSION:");

                    salesmanDataFile >> fileInput;
                    int vertex = stoi(fileInput);

                    do
                        salesmanDataFile >> fileInput;
                    while(fileInput != "EDGE_WEIGHT_FORMAT:");

                    salesmanDataFile >> fileInput;
                    if(fileInput == "FULL_MATRIX")
                    {
                        if(graph != NULL)
                            delete graph;

                        if(useListGraph)
                            graph = new ListGraph(vertex);
                        else
                            graph = new ArrayGraph(vertex);

                        do
                            salesmanDataFile >> fileInput;
                        while(fileInput != "EDGE_WEIGHT_SECTION");

                        for(int i = 0; i < vertex; i++)
                        {
                            for(int j = 0; j < vertex; j++)
                            {
                                salesmanDataFile >> fileInput;
                                int weight = stoi(fileInput);

                                if(i != j)
                                    graph->addEdge(i, j, weight);
                            }
                        }

                        cout << "Wczytano - liczba wierzcholkow: " << vertex << endl;
                        cout << endl;
                    }
                    else
                    {
                        cout << "+++ MELON MELON MELON +++ Nieobslugiwany format " << fileInput << " +++" << endl;
                        cout << endl;
                    }

                    salesmanDataFile.close();
                }
                else
                {
                    cout << "+++ MELON MELON MELON +++ Brak pliku " << filename << " +++" << endl;
                    cout << endl;
                }
            }
            break;
            case 11:
            {
                // Jan Potocki 2017
                string filename, fileInput;
                vector<float> xCoord, yCoord;
                ifstream salesmanDataFile;

                cout << "Podaj nazwe pliku: ";
                cin >> filename;

                salesmanDataFile.open(filename.c_str());
                if(salesmanDataFile.is_open())
                {
                    do
                        salesmanDataFile >> fileInput;
                    while(fileInput != "DIMENSION:");

                    salesmanDataFile >> fileInput;
                    int vertex = stoi(fileInput);

                    do
                        salesmanDataFile >> fileInput;
                    while(fileInput != "EDGE_WEIGHT_TYPE:");

                    salesmanDataFile >> fileInput;
                    if(fileInput == "EUC_2D")
                    {
                        if(graph != NULL)
                            delete graph;

                        if(useListGraph)
                            graph = new ListGraph(vertex);
                        else
                            graph = new ArrayGraph(vertex);

                        do
                            salesmanDataFile >> fileInput;
                        while(fileInput != "NODE_COORD_SECTION");

                        for(int i = 0; i < vertex; i++)
                        {
                            salesmanDataFile >> fileInput;

                            salesmanDataFile >> fileInput;
                            xCoord.push_back(stof(fileInput));

                            salesmanDataFile >> fileInput;
                            yCoord.push_back(stof(fileInput));
                        }

                        // To daloby sie zrobic optymalniej (macierz symetryczna), ale nie chce mi sie ...
                        // ..wole zoptymalizować czas programowania ;-)
                        for(int i = 0; i < vertex; i++)
                        {
                            for(int j = 0; j < vertex; j++)
                            {
                                if(i != j)
                                {
                                    float xDiff = xCoord.at(i) - xCoord.at(j);
                                    float yDiff = yCoord.at(i) - yCoord.at(j);
                                    int weight = nearbyint(sqrt(xDiff * xDiff + yDiff * yDiff));

                                    graph->addEdge(i, j, weight);
                                }
                            }
                        }

                        cout << "Wczytano - liczba wierzcholkow: " << vertex << endl;
                        cout << endl;
                    }
                    else
                    {
                        cout << "+++ MELON MELON MELON +++ Nieobslugiwany format " << fileInput << " +++" << endl;
                        cout << endl;
                    }

                    salesmanDataFile.close();
                }
                else
                {
                    cout << "+++ MELON MELON MELON +++ Brak pliku " << filename << " +++" << endl;
                    cout << endl;
                }
            }
            break;
            case 0:
            {
            }
            break;
            default:
            {
                cout << "Nieprawidlowy wybor" << endl;
                cout << endl;
            }
        }
    } while(salesmanSelection != 0);

    if(graph != NULL)
        delete graph;

    cout << "Konczenie..." << endl;
    // Easter egg :-P
    cout << '"' << "Myslak Stibbons niepokoil sie HEX-em." << endl;
    cout << "Nie wiedzial, jak dziala, chociaz wszyscy uwazali, ze wie." << endl;
    cout << "Oczywiscie, calkiem niezle orientowal sie w niektorych elementach;" << endl;
    cout << "byl tez pewien, ze HEX mysli o problemach, przeksztalcajac je" << endl;
    cout << "w liczby i mielac ..." << '"' << endl;
    cout << "(Terry Pratchett, " << '"' << "Wiedzmikolaj" << '"' << ", tlumaczenie Piotr Cholewa)" << endl;
    cout << endl;

    return 0;
}
