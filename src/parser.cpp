#include "../include/parser.hpp"

using namespace std;

Parser::Parser() = default;

Parser::~Parser() = default;

vector<Image *> *Parser::readInputFile(const string &fileName) {
    uint32_t magicNumber, imageNumber, rowNumber, columnNumber;
    ifstream ifs(fileName, ios::binary);

    auto imageVector = new vector<Image *>;

    if (!ifs.is_open()) {
        cerr << "Failed to open the input file!" << endl;
        return imageVector;
    }

    cout << "Successfully opened the input file!" << endl;

    ifs.read(reinterpret_cast<char *>(&magicNumber), sizeof(int));
    magicNumber = ntohl(magicNumber);

    ifs.read(reinterpret_cast<char *>(&imageNumber), sizeof(int));
    imageNumber = ntohl(imageNumber);

    ifs.read(reinterpret_cast<char *>(&rowNumber), sizeof(int));
    rowNumber = ntohl(rowNumber);

    ifs.read(reinterpret_cast<char *>(&columnNumber), sizeof(int));
    columnNumber = ntohl(columnNumber);

    cout << "Magic number is: " << magicNumber << endl;
    cout << "Image number is: " << imageNumber << endl;
    cout << "Row number is: " << rowNumber << endl;
    cout << "Column number is: " << columnNumber << endl;

    const uint32_t imageSize = rowNumber * columnNumber;
    vector<unsigned char> inputVector(imageSize);

    for (uint i = 0; i < imageNumber; i++) {

        ifs.read(reinterpret_cast<char *>(inputVector.data()), imageSize);

        auto coords = new vector<double>;
        for (auto uc : inputVector) {
            auto d = static_cast<double>(uc);
            coords->push_back(d);
        }

        imageVector->push_back(new Image(i+1, coords, 0));
    }

    cout << "Finished reading images!" << endl;

    ifs.close();
    return imageVector;
}

vector<Image *> *Parser::readQueryFile(const string &fileName) {
    uint32_t magicNumber, imageNumber, rowNumber, columnNumber;
    ifstream ifs(fileName, ios::binary);

    auto queryImages = new vector<Image *>;

    if (!ifs.is_open()) {
        cerr << "Failed to open the query file!" << endl;
        return queryImages;
    }

    ifs.read(reinterpret_cast<char *>(&magicNumber), sizeof(int));
    magicNumber = ntohl(magicNumber);

    ifs.read(reinterpret_cast<char *>(&imageNumber), sizeof(int));
    imageNumber = ntohl(imageNumber);

    ifs.read(reinterpret_cast<char *>(&rowNumber), sizeof(int));
    rowNumber = ntohl(rowNumber);

    ifs.read(reinterpret_cast<char *>(&columnNumber), sizeof(int));
    columnNumber = ntohl(columnNumber);

    const uint32_t imageSize = rowNumber * columnNumber;
    vector<unsigned char> inputVector(imageSize);

    for (int i = 0; i < 10; i++) {
        ifs.read(reinterpret_cast<char *>(inputVector.data()), imageSize);

        auto coords = new vector<double>;
        for (auto uc : inputVector) {
            auto d = static_cast<double>(uc);
            coords->push_back(d);
        }
        queryImages->push_back(new Image(i+1, coords, 0));
    }

    ifs.close();
    return queryImages;
}

Clustering *Parser::readClusterConf(const string &fileName, const string &outputFile) {
    int clusters = 3;
    int L = 3;
    int kLSH = 4;
    int M = 10;
    int kCube = 3;
    int probes = 2;

    ifstream ifs(fileName);

    if (!ifs.is_open()) {
        cerr << "Failed to open the cluster conf file!" << endl;
        return nullptr;
    }

    string line;
    while (getline(ifs, line)) {
        istringstream iss(line);
        string token;

        while (iss >> token) {
            if (token == "number_of_clusters:") {
                iss >> clusters;
            } else if (token == "number_of_vector_hash_tables:") {
                iss >> L;
            } else if (token == "number_of_vector_hash_functions:") {
                iss >> kLSH;
            } else if (token == "max_number_M_hypercube:") {
                iss >> M;
            } else if (token == "number_of_hypercube_dimensions:") {
                iss >> kCube;
            } else if (token == "number_of_probes:") {
                iss >> probes;
            }
        }
    }

    ifs.close();

    return new Clustering(clusters, L, kLSH, M, kCube, probes, outputFile);
}