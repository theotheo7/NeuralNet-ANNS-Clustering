#include <iostream>
#include <unistd.h>
#include <cstring>
#include <vector>

#include "../include/parser.hpp"
#include "../include/mrng.hpp"
#include "../include/gnns.hpp"
#include "../include/neuralnet.hpp"

using namespace std;

int main(int argc, char **argv) {
    int opt;
    string newInputFile, newQueryFile, inputFile, queryFile, outputFile, confFile;
    int k = 50, m = 0;
    bool complete = false;

    chrono::duration<double> tCluster{};

    string s;

    while((opt = getopt(argc, argv, "d:i:q:t:c:m:o:")) != -1) {
        switch (opt) {
            case 'd':
                inputFile = optarg;
                break;
            case 'i':
                newInputFile = optarg;
                break;
            case 'q':
                queryFile = optarg;
                break;
            case 't':
                newQueryFile = optarg;
                break;
            case 'c':
                if (strcmp(optarg, "omplete")== 0) {
                    complete = true;
                } else {
                    confFile = optarg;
                }
                break;
            case 'm':
                m = stoi(optarg);
                break;
            case 'o':
                outputFile = optarg;
                break;
            default:
                abort();
        }
    }

    if (inputFile.empty()) {
        cout << "Please provide input file!" << endl;
        cin >> inputFile;
    }

    if (newInputFile.empty()) {
        cout << "Please provide new input file!" << endl;
        cin >> newInputFile;
    }

    if (queryFile.empty()) {
        cout << "Please provide query file!" << endl;
        cin >> queryFile;
    }

    if (newQueryFile.empty()) {
        cout << "Please provide new query file!" << endl;
        cin >> newQueryFile;
    }

    if (outputFile.empty()) {
        cout << "Please provide output file!" << endl;
        cin >> outputFile;
    }

    do {
        auto parser = new Parser();

        // Read input and query images from files
        vector<Image *> *inputImages = parser->readInputFile(inputFile);
        vector<Image *> *newInputImages = parser->readInputFile(newInputFile);
        vector<Image *> *queryImages = parser->readQueryFile(queryFile);
        vector<Image *> *newQueryImages = parser->readQueryFile(newQueryFile);

        auto neuralnet = new NeuralNet(inputImages, newInputImages, outputFile);

        if (m == 0) {
            // Brute force
            for (auto query : *newQueryImages) {
                neuralnet->bruteForce(query, queryImages);
            }

            neuralnet->outputTimeMAF((int)queryImages->size());

            delete neuralnet;

        } else if (m == 1) {
            // Initialize GNNS object
            auto gnns = new GNNS(30, 1, 1, newInputImages, outputFile);

            // Graph construction
            gnns->constructGraph(newInputImages, k);

            for (auto query : *newQueryImages) {
                // Run query
                gnns->searchLatent(query, queryImages, inputImages);
            }

            gnns->outputTimeMAF((int)queryImages->size());

            delete gnns;

        } else if (m == 2) {
            // Initialize MRNG object
            auto mrng = new MRNG(1, 20, newInputImages, outputFile);

            // Graph construction
            mrng->constructGraph();

            // Find image closest to centroid to use as starting node
            mrng->findStartingNode();

            for (auto queryImage : *newQueryImages) {
                // Run query
                mrng->searchOnGraphLatent(queryImage, queryImages, inputImages);

                // Reset image checked flags
                mrng->setAllUnchecked();
            }

            // Output average time and MAF
            mrng->outputTimeMAF((int)queryImages->size());

            delete mrng;
        } else {
            auto clustering = parser->readClusterConf(confFile, outputFile);

            auto startTime = chrono::high_resolution_clock::now();
            clustering->initialize(newInputImages);

            clustering->lloyds(newInputImages, 20);

            clustering->reverse(inputImages);

            auto endTime = chrono::high_resolution_clock::now();
            tCluster = endTime - startTime;

            clustering->silhouette(inputImages);
            clustering->objectiveFunction();

            clustering->outputResults(false, "Classic", tCluster.count());

            delete clustering;
        }

        delete parser;
        for (auto image : *inputImages) {
            delete image;
        }
        delete inputImages;

        for (auto image : *newInputImages) {
            delete image;
        }
        delete newInputImages;

        for (auto image : *queryImages) {
            delete image;
        }
        delete queryImages;

        for (auto image : *newQueryImages) {
            delete image;
        }
        delete newQueryImages;

        cout << "Query finished! Would you like to start a new query? (y/n)" << endl;
        cin >> s;
        if (s == "y") {
            if (inputFile.empty()) {
                cout << "Please provide input file!" << endl;
                cin >> inputFile;
            }
            if (queryFile.empty()) {
                cout << "Please provide query file!" << endl;
                cin >> queryFile;
            }
            if (outputFile.empty()) {
                cout << "Please provide output file!" << endl;
                cin >> outputFile;
            }
        }
    } while (s != "n");

    return 0;
}