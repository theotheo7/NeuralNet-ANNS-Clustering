#ifndef NEURALNET_HPP
#define NEURALNET_HPP

#include <iostream>
#include <vector>
#include <string>

#include "image.hpp"
#include "clustering.hpp"

class NeuralNet {
private:
    std::vector<Image *> *inputImages;
    std::vector<Image *> *newInputImages;

    double totalApproximate;
    double totalTrue;

    double totalAF;

    std::ofstream output;

public:

    NeuralNet(std::vector<Image *> *, std::vector<Image *> *, const std::string&);
    ~NeuralNet();

    void bruteForce(Image *, std::vector<Image *> *);

    std::pair<uint, double> getTrueNearestNeighbor(Image *);

    void outputResults(std::pair<uint, double>, double, Image *);
    void outputTimeMAF(int);
};

#endif