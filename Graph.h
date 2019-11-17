#ifndef GRAPH_H
#define GRAPH_H
#include <vector>


class Graph
{
    public:
        Graph();
        virtual ~Graph();
        virtual bool addEdge(unsigned v, unsigned w, unsigned weight) = 0;
        virtual bool removeEdge(unsigned v, unsigned w) = 0;
        virtual unsigned getWeight(unsigned v, unsigned w) = 0;
        unsigned getVertexNumber();
        virtual void displayGraph() = 0;
        static void randomGenerateFullGraph(Graph &graph, unsigned maxWeight);
        static std::vector<unsigned> travellingSalesmanBruteForce(Graph &graph);
        static std::vector<unsigned> travellingSalesmanBranchAndBound(Graph &graph);
        static std::vector<unsigned> travellingSalesmanGreedy(Graph &graph, unsigned startVertex);
        static std::vector<unsigned> travellingSalesmanHybrid(Graph &graph);
        static std::vector<unsigned> travellingSalesmanRandom(Graph &graph);
        static std::vector<unsigned> travellingSalesmanTabuSearch(Graph &graph, unsigned tabuSteps, bool diversification, int iterationsToRestart, unsigned minStopTime, unsigned threadsNumber);

    protected:
        unsigned vertexNumber;

    private:
        static void travellingSalesmanTabuSearchEngine(Graph &graph, unsigned tabuSteps, bool diversification, int iterationsToRestart, unsigned minStopTime, std::vector<unsigned> startRoute, std::vector<unsigned> &result, int &resultLength);

        class RouteComparison
        {
            public:
                bool operator() (const std::vector<unsigned>& lhs, const std::vector<unsigned>& rhs) const
                {
                    return (lhs.at(0) > rhs.at(0));
                }
        };
};

#endif // GRAPH_H
