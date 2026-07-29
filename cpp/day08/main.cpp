#include <cmath>
#include <iostream>
#include <print>
#include <vector>
#include "../common/utils.h"

class Point3D {
public:
    Point3D(long x_val, long y_val, long z_val)
        : x(x_val), y(y_val), z(z_val) {}

    long getX() const { return x; }
    long getY() const { return y; }
    long getZ() const { return z; }
    void setCoordinates(long x_val, long y_val, long z_val) {
        x = x_val;
        y = y_val;
        z = z_val;
    }

    double distanceTo(const Point3D& other) const {
        return std::sqrt(std::pow(x - other.x, 2) +
                         std::pow(y - other.y, 2) +
                         std::pow(z - other.z, 2));
    }

    bool operator==(const Point3D& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    size_t hash() const {
        return std::hash<size_t>()(x) ^ std::hash<size_t>()(y) ^ std::hash<size_t>()(z);
    }

    std::string toString() const {
        return "Point(" + std::to_string(x) + "," +
               std::to_string(y) + "," + std::to_string(z) + ")";
    }

private:
    long x;
    long y;
    long z;
};

double distanceBetween(const Point3D& p1, const Point3D& p2) {
    if (p1 == p2) {
        return 0.0;
    }
    return p1.distanceTo(p2);
}

void print2d(const std::vector<std::vector<double>>& matrix) {
    // truncate doubles to 2 decimal places for printing
    for (const auto& row : matrix) {
        for (const auto& val : row) {
            std::print("{:.2f} ", val);
        }
        std::println();
    }
}

int main() {
    std::vector<Point3D> points;
    for (const auto& line : utils::readLinesFromFile(utils::getInputFilePath("inputs/day8.txt"))) {
        long x, y, z;
        if (sscanf(line.c_str(), "%ld,%ld,%ld", &x, &y, &z) == 3) {
            points.emplace_back(x, y, z);
        }
    }
    // std::cout << "Read " << points.size() << " points." << std::endl;

    // create a distance matrix
    std::vector<std::vector<double>> distanceMatrix(points.size(), std::vector<double>(points.size(), 0.0));
    for (size_t i = 0; i < points.size(); ++i) {
        for (size_t j = 0; j < points.size(); ++j) {
            distanceMatrix[i][j] = distanceBetween(points[i], points[j]);
        }
    }

    // print2d(distanceMatrix);

    // std::cout << "Created distance matrix of size " << distanceMatrix.size() << "x" << (distanceMatrix.empty() ? 0 : distanceMatrix[0].size()) << std::endl;

    // copy points into clusters vector
    std::vector<std::vector<Point3D>> clusters;
    for (const auto& point : points) {
        clusters.push_back({point});
    }
    // connect the 1000 closest points into clusters
    for (size_t i = 0; i < 1000; ++i) {
        double minDistance = std::numeric_limits<double>::max();
        size_t minI = 0, minJ = 0;
        for (size_t j = 0; j < points.size(); ++j) {
            for (size_t k = j + 1; k < points.size(); ++k) {
                if (distanceMatrix[j][k] < minDistance) {
                    minDistance = distanceMatrix[j][k];
                    minI = j;
                    minJ = k;
                }
            }
        }
        
        // std::cout << "( " << points[minI].toString() << ',' << points[minJ].toString()
        //           << " ) -> " << minDistance << std::endl;
        
        // remove this distance from the matrix to avoid re-connecting
        distanceMatrix[minI][minJ] = std::numeric_limits<double>::max();
        distanceMatrix[minJ][minI] = std::numeric_limits<double>::max();

        // check if points are in existing clusters
        // if one is in a cluster and the other is not, add the non-member to that cluster
        // if both are in different clusters, merge the clusters
        // if neither are in a cluster, create a new cluster
        size_t clusterIndexI = clusters.size();
        size_t clusterIndexJ = clusters.size();
        for (size_t c = 0; c < clusters.size(); ++c) {
            for (const auto& p : clusters[c]) {
                if (p == points[minI]) {
                    clusterIndexI = c;
                }
                if (p == points[minJ]) {
                    clusterIndexJ = c;
                }
            }
        }
        
        
        if (clusterIndexI == clusters.size() && clusterIndexJ == clusters.size()) { // neither point is in a cluster
            clusters.push_back({points[minI], points[minJ]});
        } else if (clusterIndexI != clusters.size() && clusterIndexJ == clusters.size()) { // I is in a cluster, J is not
            clusters[clusterIndexI].push_back(points[minJ]);
        } else if (clusterIndexI == clusters.size() && clusterIndexJ != clusters.size()) { // J is in a cluster, I is not
            clusters[clusterIndexJ].push_back(points[minI]);
        } else if (clusterIndexI != clusterIndexJ) {
            clusters[clusterIndexI].insert(clusters[clusterIndexI].end(),
                                          clusters[clusterIndexJ].begin(),
                                          clusters[clusterIndexJ].end());
            clusters.erase(clusters.begin() + clusterIndexJ);
        }
    }

    std::cout << "Formed " << clusters.size() << " clusters." << std::endl;
    // find the biggest 3 clusters
    std::sort(clusters.begin(), clusters.end(),
              [](const std::vector<Point3D>& a, const std::vector<Point3D>& b) {
                  return a.size() > b.size();
              });

    // size_t checksum = 0;
    // std::cout << "{ ";
    // for (const auto& cluster: clusters) {
    //     checksum += cluster.size();
    //     std::cout << cluster.size() << " ";
    // }
    // std::cout << "} (" << checksum << ")" << std::endl;

    unsigned long long result = 1;
    for (size_t i = 0; i < 3 && i < clusters.size(); ++i) {
        result *= clusters[i].size();
    }
    std::cout << "Part 1: " << result << std::endl;

    // reset clusters and distance matrix
    clusters.clear();
    for (const auto& point : points) {
        clusters.push_back({point});
    }
    for (size_t i = 0; i < points.size(); ++i) {
        for (size_t j = 0; j < points.size(); ++j) {
            distanceMatrix[i][j] = distanceBetween(points[i], points[j]);
        }
    }

    size_t lastMinI=0, lastMinJ=0;
    while (clusters.size() > 1) {
        double minDistance = std::numeric_limits<double>::max();
        size_t minI = 0, minJ = 0;
        for (size_t j = 0; j < points.size(); ++j) {
            for (size_t k = j + 1; k < points.size(); ++k) {
                if (distanceMatrix[j][k] < minDistance) {
                    minDistance = distanceMatrix[j][k];
                    minI = j;
                    minJ = k;
                }
            }
        }
        
        // std::cout << "( " << points[minI].toString() << ',' << points[minJ].toString()
        //           << " ) -> " << minDistance << std::endl;
        
        // remove this distance from the matrix to avoid re-connecting
        distanceMatrix[minI][minJ] = std::numeric_limits<double>::max();
        distanceMatrix[minJ][minI] = std::numeric_limits<double>::max();
        lastMinI = minI;
        lastMinJ = minJ;

        // check if points are in existing clusters
        // if one is in a cluster and the other is not, add the non-member to that cluster
        // if both are in different clusters, merge the clusters
        // if neither are in a cluster, create a new cluster
        size_t clusterIndexI = clusters.size();
        size_t clusterIndexJ = clusters.size();
        for (size_t c = 0; c < clusters.size(); ++c) {
            for (const auto& p : clusters[c]) {
                if (p == points[minI]) {
                    clusterIndexI = c;
                }
                if (p == points[minJ]) {
                    clusterIndexJ = c;
                }
            }
        }
        
        
        if (clusterIndexI == clusters.size() && clusterIndexJ == clusters.size()) { // neither point is in a cluster
            clusters.push_back({points[minI], points[minJ]});
        } else if (clusterIndexI != clusters.size() && clusterIndexJ == clusters.size()) { // I is in a cluster, J is not
            clusters[clusterIndexI].push_back(points[minJ]);
        } else if (clusterIndexI == clusters.size() && clusterIndexJ != clusters.size()) { // J is in a cluster, I is not
            clusters[clusterIndexJ].push_back(points[minI]);
        } else if (clusterIndexI != clusterIndexJ) {
            clusters[clusterIndexI].insert(clusters[clusterIndexI].end(),
                                          clusters[clusterIndexJ].begin(),
                                          clusters[clusterIndexJ].end());
            clusters.erase(clusters.begin() + clusterIndexJ);
        }
    }

    std::cout << "Part 2: " << points[lastMinI].getX() * points[lastMinJ].getX() << std::endl;

    return 0;
}