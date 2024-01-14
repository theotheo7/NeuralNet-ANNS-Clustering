#ifndef GNNS_HPP
#define GNNS_HPP

#include <set>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

#include "../include/image.hpp"
#include "../include/lsh.hpp"


class GNNS {
    private:
        int E, R, N;
        std::vector<Image *> *data;
        // Graph representation as adjacency list
        std::vector<std::vector<Image *> *> *graph;

        double totalApproximate;
        double totalTrue;

        double totalAF;

        std::ofstream output;
    public:
        GNNS(int, int, int, std::vector<Image *> *,const std::string&);
        ~GNNS();
        //std::vector<Image*> search(Image* query);
        void search(Image* query);
        void searchLatent(Image* query, std::vector<Image *> *, std::vector<Image *> *);
        void constructGraph(std::vector<Image *> *,int);
        std::vector<double> getTrueNeighbors(Image *image);
        double getTrueNeighbor(Image *image, std::vector<Image *> *);
        void outputResults(std::vector<std::pair<Image *, double>>, std::vector<double>, Image *query);
        void outputTimeMAF(int);
};

#endif