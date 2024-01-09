#include "../include/cluster.hpp"

using namespace std;

Cluster::Cluster(uint id, vector<double> *coords) {

    this->id = id;
    this->centroid = coords;
    this->images = new vector<Image *>;

}

Cluster::~Cluster() {
    delete centroid;
    delete images;
}

uint Cluster::getId() {
    return this->id;
}

void Cluster::setId(uint newId) {
    this->id = newId;
}

vector<double> *Cluster::getCentroid() {
    return this->centroid;
}

void Cluster::setCentroid(vector<double> *newCentroid) {
    this->centroid = newCentroid;
}

vector<Image *> *Cluster::getImages() {
    return this->images;
}

void Cluster::setImages(std::vector<Image *> *newImages) {
    this->images = newImages;
}

void Cluster::assign(void *pointer) {
    auto image = (Image *) pointer;

    this->images->push_back(image);
    image->setCluster(this->id);
}

void Cluster::markAllAssigned() {
    for (auto image : *images) {
        image->setAssigned(true);
    }
}

void Cluster::removeImage(Image *image) {
    for (int i = 0; i < (int) images->size(); i++) {
        if (images->at(i)->getId() == image->getId()) {
            images->erase(images->begin() + i);
            break;
        }
    }
}