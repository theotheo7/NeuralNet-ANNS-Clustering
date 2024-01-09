#include <iostream>
#include <string>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <algorithm>

#include <random>

#include "../include/parser.hpp"

using namespace std;

int main(int argc, char **argv) {
    int opt;
    bool complete = false;
    string method = "Classic";

    chrono::duration<double> tCluster{};

    string inputFile, confFile, outputFile;

    while ((opt = getopt(argc, argv, "i:c:o:m:complete")) != -1) {
        switch(opt) {
            case 'i':
                inputFile = optarg;
                break;
            case 'c':
                if (strcmp(optarg, "omplete")== 0) {
                    complete = true;
                } else {
                    confFile = optarg;
                }
                break;
            case 'o':
                outputFile = optarg;
                break;
            case 'm':
                method = optarg;
                break;
            case '?':
                complete = true;
                break;
            default:
                abort();
        }
    }

    if (inputFile.empty()) {
        cout << "Please provide input file!" << endl;
        cin >> inputFile;
    }
    if (outputFile.empty()) {
        cout << "Please provide output file!" << endl;
        cin >> outputFile;
    }
    if (confFile.empty()) {
        cout << "Please provide conf file!" << endl;
        cin >> confFile;
    }

    auto parser = new Parser();
    vector<Image *> *inputImages = parser->readInputFile(inputFile);
    Clustering *clustering = parser->readClusterConf(confFile, outputFile);

    auto startTime = chrono::high_resolution_clock::now();
    // Initialize using kpp
    clustering->initialize(inputImages);

    // Call specific method for assignment
    if (method == "LSH") {
        clustering->reverseLSH(inputImages);
    } else if (method == "Hypercube") {
        clustering->reverseCube(inputImages);
    } else {
        clustering->lloyds(inputImages, 20);
    }

    auto endTime = chrono::high_resolution_clock::now();

    tCluster = endTime - startTime;

    // Calculate the silhouette
    clustering->silhouette(inputImages); //silhouette

    clustering->outputResults(complete, method, tCluster.count());

    delete parser;
    delete clustering;

    for (auto image : *inputImages) {
        delete image;
    }

    delete inputImages;

    return 0;
}