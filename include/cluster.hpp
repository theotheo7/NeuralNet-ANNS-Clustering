#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include <iostream>
#include <vector>
#include <string>

#include "./image.hpp"

class Image;

class Cluster {
private:

    uint id;

    std::vector<double> *centroid;

    std::vector<Image *> *images;

public:
    Cluster(uint, std::vector<double> *);
    ~Cluster();

    uint getId();
    void setId(uint);

    std::vector<double> *getCentroid();
    void setCentroid(std::vector<double> *);

    std::vector<Image *> *getImages();
    void setImages(std::vector<Image *> *);

    void assign(void *);
    void markAllAssigned();

    void removeImage(Image *);

};

#endif