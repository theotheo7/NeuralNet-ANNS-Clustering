#include "../include/neuralnet.hpp"

using namespace std;

NeuralNet::NeuralNet(std::vector<Image *> *input, std::vector<Image *> *newInput, const std::string &outputFile) {
    this->totalApproximate = 0;
    this->totalTrue = 0;
    this->totalAF = 0;

    this->inputImages = input;
    this->newInputImages = newInput;

    output.open(outputFile, ios::trunc);
    if (!output.is_open()) {
        cerr << "Can't open output file!" << endl;
        abort();
    }
    string contents;
    contents.append("NeuralNet Results\n");
    output << contents;
}

NeuralNet::~NeuralNet() {
    if (output.is_open()) {
        output.close();
    }
}

// Brute force search function
void NeuralNet::bruteForce(Image *query, vector<Image *> *queryImages) {
    pair<uint, double> trueNearest;

    chrono::duration<double> tApproximate{}, tTrue{};

    auto startTrue = chrono::high_resolution_clock::now();
    trueNearest = getTrueNearestNeighbor(queryImages->at(query->getId() - 1));
    auto endTrue = chrono::high_resolution_clock::now();

    tTrue = endTrue - startTrue;
    this->totalTrue += tTrue.count();

    vector<pair<uint, double>> approxNeighbors;

    auto startApproximate = chrono::high_resolution_clock::now();
    for (auto image : *newInputImages) {
        approxNeighbors.emplace_back(image->getId(), dist(query->getCoords(), image->getCoords()));
    }

    sort(approxNeighbors.begin(), approxNeighbors.end(), sortPairBySecond);

    auto neighbor = inputImages->at(approxNeighbors.at(0).first - 1);
    pair<uint, double> approxNeighbor = make_pair(neighbor->getId(), dist(neighbor->getCoords(), queryImages->at(query->getId() - 1)->getCoords()));
    auto endApproximate = chrono::high_resolution_clock::now();

    tApproximate = endApproximate - startApproximate;
    this->totalApproximate += tApproximate.count();

    totalAF += approxNeighbor.second / trueNearest.second;

    outputResults(approxNeighbor, trueNearest.second, query);
}

// Find nearest neighbor in initial dimension
pair<uint, double> NeuralNet::getTrueNearestNeighbor(Image *queryImage) {
    vector<pair<uint, double>> trueNeighbors;
    for (auto image : *this->inputImages) {
        trueNeighbors.emplace_back(image->getId(), dist(queryImage->getCoords(), image->getCoords()));
    }

    sort(trueNeighbors.begin(), trueNeighbors.end(), sortPairBySecond);

    return trueNeighbors.at(0);
}

// Output results of query to file
void NeuralNet::outputResults(pair<uint, double> approxNeighbor, double trueDistance, Image *query) {
    string contents;

    if (output.is_open()) {
        contents.append("Query: " + to_string(query->getId()) + "\n");
        contents.append("Nearest neighbor-1: " + to_string(approxNeighbor.first) + "\n");
        contents.append("distanceApproximate: " + to_string(approxNeighbor.second) + "\n");
        contents.append("distanceTrue: " + to_string(trueDistance) + "\n");

        contents.append("\n");

        output << contents;
    }
}

// Output average time and mean approximation factor
void NeuralNet::outputTimeMAF(int querySize) {
    string contents;

    if (output.is_open()) {
        contents.append("tAverageApproximate: " + to_string(this->totalApproximate / querySize) + "\n");
        contents.append("tAverageTrue: " + to_string(this->totalTrue / querySize) + "\n");
        contents.append("Mean AF: " + to_string(this->totalAF / querySize) + "\n");

        output << contents;
    }
}