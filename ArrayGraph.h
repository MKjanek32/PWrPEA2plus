#ifndef ARRAYGRAPH_H
#define ARRAYGRAPH_H

#include "Graph.h"

class ArrayGraph : public Graph
{
    public:
        ArrayGraph(unsigned vertexNumber);
        virtual ~ArrayGraph();
        bool addEdge(unsigned v, unsigned w, unsigned weight);
        bool removeEdge(unsigned v, unsigned w);
        unsigned getWeight(unsigned v, unsigned w);
        void displayGraph();

    protected:

    private:
        unsigned **graphMatrix;
        unsigned *graphArray;

};

#endif // ARRAYGRAPH_H
