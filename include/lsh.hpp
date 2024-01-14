#ifndef LSH_HPP
#define LSH_HPP

#include <vector>
#include <list>
#include <utility>
#include <queue>
#include <algorithm>
#include <unordered_set>
#include <iostream>
#include <fstream>

#include "image.hpp"
#include "hash_table.hpp"
#include "hash_function.hpp"

class LSH
{
private:
    int k, L, N, R, w;
    std::vector<Image *> *data;
    std::vector<std::pair<HashTable *, std::vector<HashFunction *>*>> hashTables;
    std::vector<std::vector<int>> r;

    double totalApproximate;
    double totalTrue;

    double totalAF;

    std::ofstream output;

public:
    // Constructor
    LSH(int k, int L, int N, int R, std::vector<Image *> *data, const std::string&);
    LSH(int k, int L, int R, std::vector<Image *> *data);
    LSH(int, int, std::vector<Image *> *);

    void insert(Image *);
    
    // Approximate k-Nearest Neighbors method
    void query(Image* q);

    std::vector<double> getTrueNeighbors(Image *);

    std::vector<Image *> *reverseSearch(std::vector<double> *, int);

    Image *getNeighbor(Image *);
    std::vector<Image *>getNeighborsGNNS(Image *, int);

    void outputResults(std::vector<std::pair<uint, double>>, std::vector<double>, const std::set<uint>&, Image *);
    void outputTimeMAF(int);

    // Destructor
    ~LSH();
};

#endif
