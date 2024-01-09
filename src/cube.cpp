#include "../include/cube.hpp"

using namespace std;

HyperCube::HyperCube(int k, int M, int probes, int N, int R, vector<Image *> *data, string outputFile) {

    this->k = k;
    this->M = M;
    this->probes = probes;
    this->N = N;
    this->R = R;
    this->data = data;

    this->w = 10;

    this->vertices.reserve(k);
    this->cube = new HashTable((int)pow(2,k));

    for (int i = 0; i < k; i++) {
        pair<HashFunction *, unordered_map<uint, char> *> temp = make_pair(new HashFunction(w), new unordered_map<uint, char>());
        this->vertices.push_back(temp);
    }

    for (auto image : *data) {
        insert(image);
    }

    output.open(outputFile, ios::trunc);
    if (!output.is_open()) {
        cerr << "Can't open output file!" << endl;
        abort();
    }
}

HyperCube::HyperCube(int k, int M, int probes, std::vector<Image *> *data) {

    this->k = k;
    this->M = M;
    this->probes = probes;
    this->w = 10;
    this->data = data;

    this->vertices.reserve(k);
    this->cube = new HashTable((int)pow(2, k));

    for (int i = 0; i < k; i++) {
        pair<HashFunction *, unordered_map<uint, char> *> temp = make_pair(new HashFunction(w), new unordered_map<uint, char>());
        this->vertices.push_back(temp);
    }

    for (auto image : *data) {
        insert(image);
    }
}

HyperCube::~HyperCube() {
    delete cube;

    for (auto it : vertices) {
        delete it.first;
        delete it.second;
    }

    if (output.is_open()) {
        output.close();
    }
}

string HyperCube::project(vector<double> *coords) {
    string binary;

    for (auto vertex : vertices) {
        uint h = vertex.first->h(coords);

        char bit;
        auto f = vertex.second->find(h);
        if (f == vertex.second->end()) {
            auto random = vertex.first->coinFlip();
            (random == 0) ? bit = '0' : bit = '1';
            vertex.second->insert(make_pair(h, bit));
        } else {
            bit = f->second;
        }
        binary += bit;
    }

    return binary;
}

void HyperCube::insert(void *pointer) {
    auto image = (Image *) pointer;
    string binary = project(image->getCoords());

    cube->insert(binaryToUint(binary), image);

}

void HyperCube::query(void *pointer) {
    auto image = (Image *) pointer;

    int probesChecked = 0;
    int pointsChecked;

    list<pair<uint, void *>> neighborsID;
    vector<pair<uint, double>> neighborsCube;
    list<uint> neighborsRNear;
    vector<int> *neighborVertices;

    chrono::duration<double> tCube{}, tTrue{};

    auto startTrue = chrono::high_resolution_clock::now();
    priority_queue<double, vector<double>, greater<>> neighborsTrue = getTrueNeighbors(image);
    auto endTrue = chrono::high_resolution_clock::now();

    tTrue = endTrue - startTrue;

    auto startCube = chrono::high_resolution_clock::now();
    string binary = project(image->getCoords());
    neighborsID = cube->findBucket(binaryToUint(binary));

    probesChecked++;
    pointsChecked = (int)neighborsID.size();

    int hammingDist = 1;
    while (probesChecked < probes && pointsChecked < M) {
        neighborVertices = hammingDistance(binary, hammingDist++);
        for (auto i : *neighborVertices) {
            neighborsID.splice(neighborsID.begin(), cube->findBucket(i));
            pointsChecked = (int) neighborsID.size();
            probesChecked++;
            if (probesChecked == probes || pointsChecked > M) {
                break;
            }
        }
        delete neighborVertices;
    }

    for (auto neighbor : neighborsID) {
        auto neighborImage = (Image *) neighbor.second;
        double distance = dist(neighborImage->getCoords(), image->getCoords());

        neighborsCube.emplace_back(neighborImage->getId(), distance);
        if (distance < R) {
            neighborsRNear.push_back(neighborImage->getId());
        }
    }

    sort(neighborsCube.begin(), neighborsCube.end(), sortPairBySecond);

    auto endCube = chrono::high_resolution_clock::now();

    tCube = endCube - startCube;

    outputResults(neighborsCube, neighborsTrue, neighborsRNear, image, tCube.count(), tTrue.count());
}

vector<Image *> *HyperCube::reverseSearch(vector<double> *q, int range) {
    auto neighbors = new vector<Image *>;
    list<pair<uint, void *>> neighborsID;
    vector<int> *neighborVertices;

    string binary = project(q);
    neighborsID = cube->findBucket(binaryToUint(binary));

    int probesChecked = 0;
    int pointsChecked;

    probesChecked++;
    pointsChecked = (int)neighborsID.size();

    // Gather neighboring vertices from hypercube
    int hammingDist = 1;
    while (probesChecked < probes && pointsChecked < M) {
        neighborVertices = hammingDistance(binary, hammingDist++);
        for (auto i : *neighborVertices) {
            neighborsID.splice(neighborsID.begin(), cube->findBucket(i));
            pointsChecked = (int) neighborsID.size();
            probesChecked++;
            if (probesChecked == probes || pointsChecked > M) {
                break;
            }
        }
        delete neighborVertices;
    }

    for (auto neighbor : neighborsID) {
        auto neighborImage = (Image *) neighbor.second;
        double distance = dist(neighborImage->getCoords(), q);

        if (distance < range) {
            neighbors->push_back(neighborImage);
        }
    }

    return neighbors;
}

priority_queue<double, vector<double>, greater<>> HyperCube::getTrueNeighbors(void *pointer) {
    auto image = (Image *) pointer;
    priority_queue<double, vector<double>, greater<>> neighborsTrue;

    for (auto it : *this->data) {
        neighborsTrue.push(dist(it->getCoords(), image->getCoords()));
    }

    return neighborsTrue;
}

void HyperCube::outputResults(vector<pair<uint, double>> neighborsCube, priority_queue<double, vector<double>, greater<>> neighborsTrue, const list<uint>& neighborsRNear, void *qImage, double tCube, double tTrue) {
    auto image = (Image *) qImage;

    string contents;

    if (output.is_open()) {
        contents.append("Query: " + to_string(image->getId()) + "\n");

        if (neighborsCube.empty()) {
            contents.append("\n");
        } else {
            for (int i = 0; i < N; i++) {
                contents.append("Nearest neighbor-" + to_string(i+1) + ": " + to_string(neighborsCube[i].first) + "\n");
                contents.append("distanceHypercube: " + to_string(neighborsCube[i].second) + "\n");
                contents.append("distanceTrue: " + to_string(neighborsTrue.top()) + "\n");
                neighborsTrue.pop();
            }

            contents.append("tCube: " + to_string(tCube) + "\n");
            contents.append("tTrue: " + to_string(tTrue) + "\n");
            contents.append("R-near neighbors:\n");

            for (auto r : neighborsRNear) {
                contents.append(to_string(r) + "\n");
            }
        }

        contents.append("\n");

        output << contents;
    }

}