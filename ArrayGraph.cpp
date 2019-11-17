#include "ArrayGraph.h"
#include <iostream>

ArrayGraph::ArrayGraph(unsigned vertexNumber)
{
    //ctor
    this->vertexNumber = vertexNumber;
    graphMatrix = new unsigned*[vertexNumber];
    graphArray = new unsigned[vertexNumber*vertexNumber];
    // W ten sposob cala tablica bedzie w pamieci w jednej czesci
    // (mniej chybien w odwolaniach procesora do cache)

    for(int i = 0; i < vertexNumber; i++)
    {
        graphMatrix[i] = graphArray + i * vertexNumber;

        for(int j = 0; j < vertexNumber; j++)
            graphMatrix[i][j] = 0;
    }
}

ArrayGraph::~ArrayGraph()
{
    //dtor
    delete graphArray;
    delete graphMatrix;
}

bool ArrayGraph::addEdge(unsigned v, unsigned w, unsigned weight)
{
    if(weight >= 1000)
        // Waga krawedzi musi byc mniejsza od 1000
        weight = 900;

    if(graphMatrix[v][w] > 0)
        return false;
    else
    {
        graphMatrix[v][w] = weight;
        return true;
    }
}

bool ArrayGraph::removeEdge(unsigned v, unsigned w)
{
    if(graphMatrix[v][w] == 0)
        return false;
    else
    {
        graphMatrix[v][w] = 0;
        return true;
    }
}

unsigned ArrayGraph::getWeight(unsigned v, unsigned w)
{
    return graphMatrix[v][w];
}

void ArrayGraph::displayGraph()
{
    for(int i = 0; i < vertexNumber; i++)
    {
        for(int j = 0; j < vertexNumber; j++)
        {
            std::cout << graphMatrix[i][j] << '\t';
        }
        std::cout << std::endl;
    }
}
