#include <iostream>
#include <vector>
#include <set>
#include "utils.h"

size_t makeRect(std::pair<long, long> topLeft, std::pair<long, long> bottomRight) {
    size_t width = std::abs(bottomRight.first - topLeft.first) + 1;
    size_t height = std::abs(bottomRight.second - topLeft.second) + 1;
    return width * height;
}

// std::vector<std::pair<long, long>> getEdgePoints(const std::pair<long, long>& topLeft,
//                                             const std::pair<long, long>& bottomRight) {
//     std::vector<std::pair<long, long>> edges;
//     for (long x = topLeft.first; x <= bottomRight.first; ++x) {
//         edges.emplace_back(x, topLeft.second);
//         edges.emplace_back(x, bottomRight.second);
//     }
//     for (long y = topLeft.second; y <= bottomRight.second; ++y) {
//         edges.emplace_back(topLeft.first, y);
//         edges.emplace_back(bottomRight.first, y);
//     }
//     return edges;
// }

std::vector<std::pair<long, long>> pointsBetween(const std::pair<long,long>& p1,
                                                 const std::pair<long,long>& p2) {
    std::vector<std::pair<long, long>> points;
    for (long x = std::min(p1.first, p2.first); x <= std::max(p1.first, p2.first); ++x) {
        for (long y = std::min(p1.second, p2.second); y <= std::max(p1.second, p2.second); ++y) {
            points.emplace_back(x, y);
        }
    }
    return points;
}

int main() {
    std::vector<std::pair<long, long>> coordinates;
    for (const auto& line : utils::readLinesFromFile("sample.txt")) {
        long x, y;
        if (sscanf(line.c_str(), "%ld,%ld", &x, &y) != 2) {
            std::cerr << "Invalid line format: " << line << std::endl;
            continue;
        }
        coordinates.emplace_back(x, y);
    }

    // find the largest rectangle possible from the coordinates
    size_t maxArea = 0;
    for (size_t i = 0; i < coordinates.size(); ++i) {
        for (size_t j = 0; j < coordinates.size(); ++j) {
            if (i == j) continue;
            size_t area = makeRect(coordinates[i], coordinates[j]);
            if (area > maxArea) {
                maxArea = area;
            }
        }
    }
    std::cout << "Part 1: " << maxArea << std::endl;
    
    // find the largest rectangle possible whose points are all
    // within or on rectangles formed by the lines drawn between each pair of coordinates
    maxArea = 0;
    std::set<std::pair<long, long>> allPoints;
    for (size_t i = 0; i < coordinates.size(); ++i) {
        for (size_t j = i + 1; j <= coordinates.size(); ++j) {
            if (j == coordinates.size()) j=0;
            
            // insert the line of points between coordinates[i] and coordinates[j]
            auto points = pointsBetween(coordinates[i], coordinates[j]);
            allPoints.insert(points.begin(), points.end());
        }
    }

    // check if any of the lines we've inserted have formed the bounds of a rectangle
    for (const auto& p1 : allPoints) {
        for (const auto& p2 : allPoints) {
            if (p1 == p2) continue;
            size_t area = makeRect(p1, p2);
            bool allCornersExist =
                allPoints.count({p1.first, p2.second}) > 0 &&
                allPoints.count({p2.first, p1.second}) > 0;
            if (allCornersExist && area > maxArea) {
                maxArea = area;
            }
        }
    }

    std::cout << "Part 2: " << maxArea << std::endl;
    
    return 0;
}