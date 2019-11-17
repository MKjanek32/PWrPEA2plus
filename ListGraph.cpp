#include "ListGraph.h"
#include <iostream>

ListGraph::ListGraph(unsigned vertexNumber)
{
    //ctor
    this->vertexNumber = vertexNumber;
    graphList = new element*[vertexNumber];

    for(int i = 0; i < vertexNumber; i++)
        graphList[i] = NULL;
}

ListGraph::~ListGraph()
{
    //dtor
    for(int i = 0; i < vertexNumber; i++)
    {
        if(graphList[i] != NULL)
        {
            element *position = graphList[i];
            do
            {
                element *next = position->next;
                delete position;
                position = next;
            }
            while(position != NULL);
        }
    }

    delete[] graphList;
}

bool ListGraph::addEdge(unsigned v, unsigned w, unsigned weight)
{
    if(weight >= 1000)
        // Waga krawedzi musi byc mniejsza od 1000
        weight = 900;

    if(graphList[v] == NULL)
    {
        graphList[v] = new element;
        graphList[v]->vertex = w;
        graphList[v]->weight = weight;
        graphList[v]->next = NULL;

        return true;
    }
    else
    {
        bool isAlready = false;
        element *next = graphList[v];
        element *position = NULL;
        do
        {
            position = next;
            next = next->next;
            if(position->vertex == w)
            {
                isAlready = true;
                break;
            }
        }
        while(next != NULL);

        if(!isAlready)
        {
            element *newEdge = new element;
            newEdge->vertex = w;
            newEdge->weight = weight;
            newEdge->next = NULL;
            position->next = newEdge;

            return true;
        }
        else
            return false;
    }
}

bool ListGraph::removeEdge(unsigned v, unsigned w)
{
    if(graphList[v] == NULL)
        return false;
    else
    {
        bool isAlready = false;
        element *next = graphList[v];
        element *position = NULL;
        element *prev = NULL;
        do
        {
            prev = position;
            position = next;
            next = next->next;
            if(position->vertex == w)
            {
                isAlready = true;
                break;
            }
        }
        while(next != NULL);

        if(!isAlready)
            return false;
        else
        {
            delete position;
            if(prev != NULL)
                prev->next = next;
            else
                graphList[v] = next;

            return true;
        }
    }
}

unsigned ListGraph::getWeight(unsigned v, unsigned w)
{
    if(graphList[v] == NULL)
        return 0;
    else
    {
        bool isAlready = false;
        element *next = graphList[v];
        element *position = NULL;
        do
        {
            position = next;
            next = next->next;
            if(position->vertex == w)
            {
                isAlready = true;
                break;
            }
        }
        while(next != NULL);

        if(!isAlready)
            return 0;
        else
            return position->weight;
    }
}

void ListGraph::displayGraph()
{
    for(int i = 0; i < vertexNumber; i++)
    {
        std::cout << i << " -> ";
        if(graphList[i] != NULL)
        {
            element *next = graphList[i];
            element *position = NULL;
            do
            {
                position = next;
                next = next->next;
                std::cout << position->vertex << '@' << position->weight << ' ';
            }
            while(next != NULL);
        }
        std::cout << std::endl;
    }
}
