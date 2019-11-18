#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cstring>
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

// Wykorzystanie reprezentacji grafu w postaci list sasiedztwa...
// ...zamiast (domyslnie) macierzy sasiedztwa
// (wolniejsze obliczenia, mniejsze uzycie pamieci)
bool useListGraph = false;

// Domyslna kadencja tabu search - wybor automatyczny
unsigned tabuLength = 0;
// Domyslny stan dywersyfikacji
bool tabuDiversification = true;
// Domyslne kryterium dywersyfikacji, liczba iteracji bez poprawy
int tabuIterationsToRestart = 10000;
// Domyslny czas zatrzymania algorytmu tabu search [s]
unsigned tabuStopTime = 60;

// Domyslna liczba watkow tabu search
unsigned tabuThreadsNumber = 2;

void parseTSPLIB_FULL_MATRIX(const char *filename, Graph **graph)
{
    // Jan Potocki 2017
    string fileInput;
    ifstream salesmanDataFile;

    salesmanDataFile.open(filename);
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
            if(*graph != NULL)
                delete *graph;

            if(useListGraph)
                *graph = new ListGraph(vertex);
            else
                *graph = new ArrayGraph(vertex);

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
                        (*graph)->addEdge(i, j, weight);
                }
            }

            cout << "Liczba wczytanych wierzcholkow: " << vertex << endl;
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

void parseTSPLIB_EUC_2D(const char *filename, Graph **graph)
{
    // Jan Potocki 2017
    string fileInput;
    vector<float> xCoord, yCoord;
    ifstream salesmanDataFile;

    salesmanDataFile.open(filename);
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
            if(*graph != NULL)
                delete *graph;

            if(useListGraph)
                *graph = new ListGraph(vertex);
            else
                *graph = new ArrayGraph(vertex);

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

                        (*graph)->addEdge(i, j, weight);
                    }
                }
            }

            cout << "Liczba wczytanych wierzcholkow: " << vertex << endl;
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

void banner()
{
    cout << "PEA Projekt 2 Plus v2.0ALPHA" << endl;
    cout << "Jan Potocki 2017-2019" << endl;
    cout << "(beerware)" << endl;
}

int main(int argc, char *argv[])
{
    Stopwatch clock;                // czasomierz
    Graph *graph = NULL;            // <- tu bedziemy zapisywac adresy przez caly program

    // Parsowanie parametrow z linii komend
    if(argc > 1)
    {
        for(int i = 1; i < argc; i++)
        {
            if(strcmp(argv[i], "-l") == 0)
            {
                useListGraph = true;
            }
            else if(strcmp(argv[i], "-t") == 0)
            {
                i++;
                int input = atoi(argv[i]);

                if(input > 0)
                    tabuThreadsNumber = input;
                else
                    cout << "+++ MELON MELON MELON +++ Nieprawidlowa liczba watkow +++" << endl << endl;
            }
            else if(strcmp(argv[i], "-fmatrix") == 0)
            {
                i++;
                parseTSPLIB_FULL_MATRIX(argv[i], &graph);
            }
            else if(strcmp(argv[i], "-feuc2d") == 0)
            {
                i++;
                parseTSPLIB_EUC_2D(argv[i], &graph);
            }
            else if(strcmp(argv[i], "-h") == 0)
            {
                banner();
                cout << endl << "Parametry:" << endl;
                cout << "-feuc2d [plik]" << "\t\t" << "wczytanie danych z pliku w formacie TSPLIB EUC_2D" << endl;
                cout << endl;
                cout << "-fmatrix [plik]" << "\t\t" << "wczytanie danych z pliku w formacie TSPLIB FULL_MATRIX" << endl;
                cout << endl;
                cout << "-l" << "\t\t\t" << "uzycie reprezentacji grafu w pamieci w postaci listy" << endl;
                cout << "\t\t\t" << "sasiedztwa (domyslnie macierz sasiedztwa)" << endl;
                cout << endl;
                cout << "-t [liczba]" << "\t\t" << "liczba watkow roboczych dla algorytmu tabu search" << endl;
                cout << endl;
                cout << "-h" << "\t\t\t" << "wyswietlenie pomocy (opisu parametrow)" << endl;
                return 0;
            }
            else
            {
                cout << "Nieprawidlowy parametr, uzyj -h aby uzyskac pomoc" << endl;
                return 0;
            }
        }
    }

    banner();
    if(useListGraph)
        cout << "Uzycie listowej reprezentacji grafu" << endl;
    else
        cout << "Uzycie macierzowej reprezentacji grafu" << endl;
    cout << endl;

    int salesmanSelection;
    do
    {
        cout << "1. Wygeneruj losowe dane" << endl;
        cout << "2. Wyswietl dane" << endl;
        cout << "3. Ustawienia TS" << endl;
        cout << "4. Tabu search" << endl;
        cout << "5. Podzial i ograniczenia" << endl;
        cout << "6. Przeglad zupelny" << endl;
        cout << "7. Automatyczne pomiary (tabu search)" << endl;
        cout << "8. Wczytaj dane z pliku TSPLIB FULL_MATRIX" << endl;
        cout << "9. Wczytaj dane z pliku TSPLIB EUC_2D" << endl;
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
                int settingsSelection;
                do
                {
                    if(tabuDiversification == true)
                        cout << "1. Przelacz dywersyfikacje" << "\t" << "(wlaczona)" << endl;
                    else
                        cout << "1. Przelacz dywersyfikacje" << "\t" << "(wylaczona)" << endl;

                    cout << "2. Kryterium dywersyfikacji" << "\t" << "(" << tabuIterationsToRestart << " iteracji)" << endl;

                    if(tabuLength == 0)
                        cout << "3. Kadencja na liscie tabu" << "\t" << "(auto)" << endl;
                    else
                        cout << "3. Kadencja na liscie tabu" << "\t" << "(" << tabuLength << ")" << endl;

                    cout << "4. Czas zatrzymania" << "\t\t" << "(" << tabuStopTime << " s)" << endl;
                    cout << "Powrot - 0" << endl;
                    cout << "Wybierz: ";
                    cin >> settingsSelection;
                    cout << endl;

                    switch(settingsSelection)
                    {
                        case 1:
                        {
                            tabuDiversification = !tabuDiversification;

                            if(tabuDiversification == true)
                                cout << "Dywersyfikacja zostala wlaczona" << endl;
                            else
                                cout << "Dywersyfikacja zostala wylaczona" << endl;
                            cout << endl;
                        }
                        break;
                        case 2:
                        {
                            cout << "Podaj nowa liczbe iteracji bez poprawy: ";
                            cin >> tabuIterationsToRestart;
                            cout << endl;
                        }
                        break;
                        case 3:
                        {
                            cout << "Podaj nowa kadencje (0 -> auto): ";
                            cin >> tabuLength;
                            cout << endl;
                        }
                        break;
                        case 4:
                        {
                            cout << "Podaj nowy czas pracy [s]: ";
                            cin >> tabuStopTime;
                            cout << endl;
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
                } while(settingsSelection != 0);
            }
            break;
            case 4:
            {
                if(graph != NULL)
                {
                    if(tabuStopTime != 0)
                    {
                        unsigned effectiveTabuLength;
                        if(tabuLength == 0)
                        {
                            effectiveTabuLength = (graph->getVertexNumber() / 10) * 10;
                            if(effectiveTabuLength == 0)
                              effectiveTabuLength = 10;
                        }
                        else
                        {
                            effectiveTabuLength = tabuLength;
                        }

                        if(tabuLength == 0)
                            cout << "Kadencja: " << effectiveTabuLength << " (auto)" << endl;
                        else
                            cout << "Kadencja: " << effectiveTabuLength << endl;

                        cout << "Czas zatrzymania algorytmu [s]: " << tabuStopTime << endl;

                        if(tabuDiversification == true)
                            cout << "Dywersyfikacja wlaczona, kryterium: " << tabuIterationsToRestart << " iteracji bez poprawy" << endl;
                        else
                            cout << "Dywersyfikacja wylaczona" << endl;

                        cout << "Liczba watkow: " << tabuThreadsNumber << endl;
                        cout << endl;

                        clock.start();
                        vector<unsigned> route = Graph::travellingSalesmanTabuSearch(*graph, effectiveTabuLength, tabuDiversification, tabuIterationsToRestart, tabuStopTime, tabuThreadsNumber);
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
            case 5:
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
            case 6:
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
            case 7:
            {
                // PEA 2
                // Jan Potocki 2017
                if(graph != NULL)
                {
                    unsigned effectiveTabuLength;
                    if(tabuLength == 0)
                    {
                        effectiveTabuLength = (graph->getVertexNumber() / 10) * 10;
                        if(effectiveTabuLength == 0)
                          effectiveTabuLength = 10;
                    }
                    else
                    {
                        effectiveTabuLength = tabuLength;
                    }

                    double measureResults[measureNumber], measureResultsDiv[measureNumber];
                    for(int i = 0; i < measureNumber; i++)
                    {
                        measureResults[i] = 0;
                        measureResultsDiv[i] = 0;
                    }

                    cout << "Pomiary dla problemu komiwojazera, tabu search" << tabuLength << endl;

                    if(tabuLength == 0)
                        cout << "Kadencja: " << effectiveTabuLength << " (auto)" << endl;
                    else
                        cout << "Kadencja: " << effectiveTabuLength << endl;

                    cout << "Kryterium dywersyfikacji: " << tabuIterationsToRestart << " iteracji bez poprawy" << endl;
                    cout << "Liczba watkow: " << tabuThreadsNumber << endl;

                    // Petla pomiarowa
                    for(int krok = 0; krok < measureIterations; krok++)
                    {
                        for(int i = 0; i < measureNumber; i++)
                        {
                            vector<unsigned> route;
                            unsigned routeLength;

                            // Bez dywersyfikacji
                            cout << "Pomiar " << measureTabuStop[i] << " [s] (" << krok + 1 << " z " << measureIterations << " bez dywersyfikacji)..." << endl;

                            route = Graph::travellingSalesmanTabuSearch(*graph, effectiveTabuLength, false, tabuIterationsToRestart, measureTabuStop[i], tabuThreadsNumber);

                            routeLength = 0;
                            for(int j = 1; j < route.size(); j++)
                                routeLength += graph->getWeight(route.at(j - 1), route.at(j));
                            measureResults[i] += routeLength;

                            // Z dywersyfikacja
                            cout << "Pomiar " << measureTabuStop[i] << " [s] (" << krok + 1 << " z " << measureIterations << " z dywersyfikacja)..." << endl;

                            route = Graph::travellingSalesmanTabuSearch(*graph, effectiveTabuLength, true, tabuIterationsToRestart, measureTabuStop[i], tabuThreadsNumber);

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
            case 8:
            {
                // Jan Potocki 2019
                string filename;

                cout << "Podaj nazwe pliku: ";
                cin >> filename;

                parseTSPLIB_FULL_MATRIX(filename.c_str(), &graph);
            }
            break;
            case 9:
            {
                // Jan Potocki 2019
                string filename;

                cout << "Podaj nazwe pliku: ";
                cin >> filename;

                parseTSPLIB_EUC_2D(filename.c_str(), &graph);
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
