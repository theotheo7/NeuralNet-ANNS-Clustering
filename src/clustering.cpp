#include "../include/clustering.hpp"

using namespace std;

Clustering::Clustering(int clusterNum, int L, int kLSH, int M, int kCube, int probes, string outputFile) {

    this->clusterNum = clusterNum;
    this->L = L;
    this->kLSH = kLSH;
    this->M = M;
    this->kCube = kCube;
    this->probes = probes;

    this->clusters = new vector<Cluster *>;
    this->silhouetteAvg = new vector<double>;
    this->objectiveFunctionValue = 0;

    output.open(outputFile);
    if (!output.is_open()) {
        cerr << "Can't open output file!" << endl;
        abort();
    }
}

Clustering::~Clustering() {
    for (auto cluster : *clusters) {
        delete cluster;
    }
    delete clusters;

    delete silhouetteAvg;

    output.close();
}

void Clustering::initialize(vector<Image *> *images) {
    vector<double> probabilities;
    vector<double> minDist;
    double distance, prob, number;

    vector<Image *> available = *images;

    // Select first centroid randomly
    clusters->push_back(selectRandomly(&available));

    // For k clusters
    for (int i = 1; i < clusterNum; i++) {
        int numOfImages = (int) available.size();

        // Initialize min distance of each image to closest centroid
        // as a "max" distance
        for (int j = 0; j < numOfImages; j++) {
            minDist.push_back(10000000);
        }

        // For every image find minimum distance to the closest centroid
        for (int j = 0; j < numOfImages; j++) {

            for (auto cluster: *clusters) {

                distance = dist(cluster->getCentroid(), available.at(j)->getCoords());
                if (distance < minDist.at(j)) {
                    minDist.at(j) = distance;
                }

            }

        }

        // Find sum of all distances
        double totalDist = 0;
        for (int j = 0; j < numOfImages; j++) {
            totalDist += minDist.at(j);
        }

        // Calculate probability of each image being picked as centroid
        for (int j = 0; j < numOfImages; j++) {
            distance = minDist.at(j);
            prob = distance / totalDist;
            probabilities.push_back(prob);
        }

        random_device rand_dev;
        mt19937 generator(rand_dev());
        uniform_real_distribution<double> distr(0.0, 1.0);
        number = distr(generator);

        // Find next centroid
        double probSum = 0;
        for (int j = 0; j < numOfImages; j++) {
            probSum += probabilities.at(j);
            if (number <= probSum) {
                clusters->push_back(new Cluster(i + 1, new vector<double>(*images->at(j)->getCoords())));
                available.erase(available.begin() + j);
                break;
            }
        }

        minDist.clear();

    }
}

Cluster *Clustering::selectRandomly(vector<Image *> *images) {
    // Seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Create a random number distribution for indices
    std::uniform_int_distribution<size_t> dist(0, images->size() - 1);

    // Generate a random index
    size_t randomIndex = dist(gen);

    Image *image = images->at(randomIndex);
    images->erase(images->begin() + randomIndex);

    // Access the random element from the vector
    return new Cluster(1, new vector<double>(*image->getCoords()));

}

void Clustering::lloyds(std::vector<Image *> *images, int maxTimes) {
    int changes;
    int numOfImages = (int) images->size();
    int acceptable = numOfImages / 500;

    do {
        cout << "Running.." << endl;
        changes = 0;

        Cluster *temp;
        for (auto image : *images) {
            temp = getClosestCentroid(image);

            if (image->getCluster() != temp->getId()) {
                changes++;
                if (image->getCluster() != 0) {
                    clusters->at(image->getCluster() - 1)->removeImage(image);
                    updateMacQueen(clusters->at(image->getCluster() - 1));
                }

                temp->assign(image);
                updateMacQueenInsert(temp);
            }

        }

        maxTimes--;

    } while (maxTimes > 0 && changes > acceptable);

}

void Clustering::reverseLSH(std::vector<Image *> *images) {
    int R = (int) minDistanceOfCentroids();
    int RMax = 100000;

    vector<Image *> *neighbors;

    auto lsh = new LSH(kLSH, L, R, images);

    do {
        cout << "Running.." << endl;
        for (auto cluster : *clusters) {
            neighbors = lsh->reverseSearch(cluster->getCentroid(), R);

            if (neighbors->empty()) {
                continue;
            }

            for (auto neighbor : *neighbors) {
                // If it has already been assigned skip it
                if (neighbor->getAssigned()) {
                    continue;
                }

                // If image is not in cluster assign it
                if (neighbor->getCluster() == 0) {
                    cluster->assign(neighbor);
                } else if (neighbor->getCluster() != 0 && !neighbor->getAssigned()) { // Image was assigned this iteration but there is conflict
                    clusters->at(neighbor->getCluster() - 1)->removeImage(neighbor);
                    getClosestCentroid(neighbor)->assign(neighbor);
                }
            }

            delete neighbors;
        }

        for (auto cluster : *clusters) {
            cluster->markAllAssigned();
            updateMacQueen(cluster);
        }

        R *= 2;

    } while (R < RMax);

    // Assign remaining unassigned images
    for (auto image : *images) {
        if (image->getCluster() == 0) {
            getClosestCentroid(image)->assign(image);
        }
    }

    delete lsh;
}

void Clustering::reverseCube(vector<Image *> *images) {
    int R = (int) minDistanceOfCentroids();
    int RMax = 100000;

    vector<Image *> *neighbors;

    auto cube = new HyperCube(kCube, M, probes, images);

    do {
        cout << "Running.." << endl;
        for (auto cluster : *clusters) {
            neighbors = cube->reverseSearch(cluster->getCentroid(), R);

            if (neighbors->empty()) {
                continue;
            }

            for (auto neighbor : *neighbors) {
                // If it has already been assigned skip it
                if (neighbor->getAssigned()) {
                    continue;
                }

                // If image is not in cluster assign it
                if (neighbor->getCluster() == 0) {
                    cluster->assign(neighbor);
                } else if (neighbor->getCluster() != 0 && !neighbor->getAssigned()) { // Image was assigned this iteration but there is conflict
                    clusters->at(neighbor->getCluster() - 1)->removeImage(neighbor);
                    getClosestCentroid(neighbor)->assign(neighbor);
                }
            }

            delete neighbors;
        }

        for (auto cluster : *clusters) {
            cluster->markAllAssigned();
            updateMacQueen(cluster);
        }

        R *= 2;

    } while (R < RMax);

    for (auto image : *images) {
        if (image->getCluster() == 0) {
            getClosestCentroid(image)->assign(image);
        }
    }

    delete cube;
}

// Function to update centroid when a single image gets assigned to its cluster
void Clustering::updateMacQueenInsert(Cluster *cluster) {
    int sizeOfCoords = (int) cluster->getCentroid()->size();

    auto coords = cluster->getCentroid();
    int imageNum = (int) cluster->getImages()->size();

    for (int i = 0; i < sizeOfCoords; i++) {
        double newImageCoord = cluster->getImages()->back()->getCoords()->at(i);
        coords->at(i) = (coords->at(i) * (imageNum - 1) + newImageCoord) / imageNum;
    }

}

// Slower function to update centroid regardless of insertion or removal of image
void Clustering::updateMacQueen(Cluster *cluster) {
    int sizeOfCoords = (int) cluster->getCentroid()->size();

    auto coords = cluster->getCentroid();
    int imageNum = (int) cluster->getImages()->size();

    for (int i = 0; i < sizeOfCoords; i++) {
        double total = 0;
        for (int j = 0; j < imageNum; j++) {
            total += cluster->getImages()->at(j)->getCoords()->at(i);
        }
        coords->at(i) = total / imageNum;
    }

}

double Clustering::minDistanceOfCentroids() {
    double minDist = 10000000;
    double distance;

    for (int i = 0; i < clusterNum; i++) {
        for (int j = 0; j < clusterNum; j++) {
            // Dont check same centroids
            if (i != j) {
                distance = dist(clusters->at(i)->getCentroid(), clusters->at(j)->getCentroid());

                if (distance < minDist) {
                    minDist = distance;
                }
            }
        }
    }

    return minDist;
}

Cluster *Clustering::getClosestCentroid(Image *image) {
    double distance;
    double min = 1000000;
    Cluster *temp;

    for (auto cluster : *clusters) {
        distance = dist(image->getCoords(), cluster->getCentroid());
        if (distance < min) {
            min = distance;
            temp = cluster;
        }
    }

    return temp;
}

double averageDistanceToCluster(Image* image, std::vector<Cluster *> *clusters) {
    double totalDistance = 0.0;
    int numImagesInCluster = 0;

    for (auto cluster : *clusters) {
        vector<Image*> *imageInCluster = cluster->getImages();
        for (auto clusterImage : *imageInCluster) {
            double distance = dist(image->getCoords(), clusterImage->getCoords());
            totalDistance += distance;
            numImagesInCluster++;
        }
    }

    if (numImagesInCluster > 0) {
        return totalDistance / numImagesInCluster;
    }

    return 0.0; // Avoid division by zero if the clusters are empty
}

double averageDistanceToNeighborCluster(Image* image, const std::vector<Cluster*>* clusters/*, unsigned int imageIndex*/) {
    unsigned int clusterIndex = image->getCluster();

    double minDistance = std::numeric_limits<double>::max(); // Initialize with a large value
    Cluster* neighborCluster = nullptr;

    for (unsigned int j = 0; j < clusters->size(); j++) {
        if (j != clusterIndex) { // Exclude the cluster the image belongs to
            double distance = dist(image->getCoords(), clusters->at(j)->getCentroid());

            if (distance < minDistance) {
                minDistance = distance;
                neighborCluster = clusters->at(j);
            }
        }
    }

    if (neighborCluster) {
        double totalDistance = 0.0;
        const std::vector<Image*>* neighborClusterImages = neighborCluster->getImages();

        for (auto neighborImage : *neighborClusterImages) {
            double distance = dist(image->getCoords(), neighborImage->getCoords()); // Calculate distance between the image and images in the neighbor cluster
            totalDistance += distance;
        }

        int neighborClusterSize = (int) neighborClusterImages->size();

        if (neighborClusterSize > 0) {
            double averageDistance = totalDistance / neighborClusterSize;
            return averageDistance;
        }
    }

    return 0.0; // No images in the neighbor cluster (or only itself), return 0
}

// Function to return clusters to initial dimension
void Clustering::reverse(std::vector<Image *> *input) {
    int sizeOfCoords = (int) input->at(0)->getCoords()->size();

    for (auto cluster : *clusters) {
        auto centroid = cluster->getCentroid();
        centroid->resize(input->at(0)->getCoords()->size());
        int clusterSize = (int) cluster->getImages()->size();
        vector<Image *> temp;

        temp = *cluster->getImages();
        auto images = cluster->getImages();
        images->clear();
        for (auto image : temp) {
            images->push_back(input->at(image->getId() - 1));
        }

        for (int i = 0; i < sizeOfCoords; i++) {
            double total = 0;
            for (int j = 0; j < clusterSize; j++) {
                total += cluster->getImages()->at(j)->getCoords()->at(i);
            }
            centroid->at(i) = total / clusterSize;
        }
    }
}

void Clustering::silhouette(std::vector<Image *> *images) {
    std::vector<double> s;
    double averageClusterSilhouette = 0.0;
    for (auto cluster : *clusters) {
        const std::vector<Image*> *clusterImages = cluster->getImages();

        for (auto image : *clusterImages) {
            double ai = averageDistanceToCluster(image, clusters);
            double bi = averageDistanceToNeighborCluster(image, clusters);

            if (std::max(ai, bi) == 0) {
                s.push_back(0.0);
            } else {
                double si = (bi - ai) / std::max(ai, bi);
                s.push_back(si);
                averageClusterSilhouette += si;
            }
        }
    }

    for (auto cluster : *clusters) {
        double average = 0.0;
        std::vector<Image*> *clusterPoints = cluster->getImages();
        for (auto clusterPoint : *clusterPoints) {
                average += s[clusterPoint->getId()];
        }
        silhouetteAvg->push_back(average/(double)clusterPoints->size());
    }
    averageClusterSilhouette /= (double) images->size();
    silhouetteAvg->push_back(averageClusterSilhouette); // add average silhouette value for whole dataset
}

void Clustering::objectiveFunction() {
    double objectiveFunctionValue = 0.0;

    // Iterate over each cluster
    for (auto cluster : *clusters) {
        const vector<double> *centroid = cluster->getCentroid();
        const vector<Image *> *images = cluster->getImages();

        // Calculate the sum of squared distances for each image in the cluster
        for (auto image : *images) {
            const vector<double> *imageCoords = image->getCoords();
            double squaredDistance = 0.0;

            // Sum up the squared distance between the image and the centroid
            for (size_t i = 0; i < centroid->size(); ++i) {
                squaredDistance += pow(imageCoords->at(i) - centroid->at(i), 2);
            }

            objectiveFunctionValue += squaredDistance;
        }
    }

    this->objectiveFunctionValue = objectiveFunctionValue;

    //cout << "Objective function: " << objectiveFunctionValue << endl;
}

void Clustering::outputResults(bool complete, const string& method, double time) {
    string contents;

    if (output.is_open()) {
        if (method == "Classic") {
            contents.append("Algorithm: Lloyds\n");
        } else if (method == "LSH") {
            contents.append("Algorithm: Range Search LSH\n");
        } else if (method == "Hypercube") {
            contents.append("Algorithm Range Search Hypercube\n");
        }

        for (auto cluster : *clusters) {
            contents.append("CLUSTER-" + to_string(cluster->getId()) + " {size: " + to_string(cluster->getImages()->size()) + ", centroid: [");
            for (auto coord : *cluster->getCentroid()) {
                contents.append(" " + to_string(coord));
            }
            contents.append(" ]\n");
        }

        contents.append("clustering_time: " + to_string(time) + "\n");

        contents.append("Silhouette: [");
        for (auto sil : *silhouetteAvg) {
            contents.append(to_string(sil) + ",");
        }
        contents.append("]\n");

        contents.append("Objective function value: " + to_string(this->objectiveFunctionValue) + "\n");

        if (complete) {
            contents.append("\n");
            for (auto cluster : *clusters) {
                contents.append("CLUSTER-" + to_string(cluster->getId()) + " {[");
                for (auto coord : *cluster->getCentroid()) {
                    contents.append(" " + to_string(coord));
                }
                contents.append(" ], ");
                for (auto image : *cluster->getImages()) {
                    contents.append(" " + to_string(image->getId()) + ",");
                }
                contents.append("}\n");
            }
        }
    }

    contents.append("\n");
    output << contents;
}