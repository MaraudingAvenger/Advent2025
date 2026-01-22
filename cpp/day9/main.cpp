#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include "utils.h"

size_t rectArea(std::pair<long, long> topLeft, std::pair<long, long> bottomRight) {
    size_t width = std::abs(bottomRight.first - topLeft.first) + 1;
    size_t height = std::abs(bottomRight.second - topLeft.second) + 1;
    return width * height;
}

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
    std::ifstream file("input.txt");
    long x, y;
    char comma;
    while (file >> x >> comma >> y) {
        coordinates.emplace_back(x, y);
    }
    file.close();

    // find the largest rectangle possible from the coordinates
    size_t maxArea = 0;
    for (size_t i = 0; i < coordinates.size(); ++i) {
        for (size_t j = 0; j < coordinates.size(); ++j) {
            if (i == j) continue;
            size_t area = rectArea(coordinates[i], coordinates[j]);
            if (area > maxArea) {
                maxArea = area;
            }
        }
    }
    std::cout << "Part 1: " << maxArea << std::endl;
    
    // Part 2: find largest rectangle with corners on polygon edges, containing only red/green tiles
    
    // Find bounding box
    long minX = coordinates[0].first, maxX = coordinates[0].first;
    long minY = coordinates[0].second, maxY = coordinates[0].second;
    for (const auto& c : coordinates) {
        minX = std::min(minX, c.first);
        maxX = std::max(maxX, c.first);
        minY = std::min(minY, c.second);
        maxY = std::max(maxY, c.second);
    }
    
    // Draw polygon edges (red tiles)
    std::set<std::pair<long, long>> redTiles;
    for (size_t i = 0; i < coordinates.size(); ++i) {
        auto p1 = coordinates[i];
        auto p2 = coordinates[(i + 1) % coordinates.size()];
        
        // Since tiles on same row or column, just draw straight line
        long minX = std::min(p1.first, p2.first);
        long maxX = std::max(p1.first, p2.first);
        long minY = std::min(p1.second, p2.second);
        long maxY = std::max(p1.second, p2.second);
        
        for (long x = minX; x <= maxX; ++x) {
            for (long y = minY; y <= maxY; ++y) {
                redTiles.insert({x, y});
            }
        }
    }
    
    // Flood fill from outside to mark white tiles
    std::set<std::pair<long, long>> whiteTiles;
    std::queue<std::pair<long, long>> q;
    q.push({minX - 1, minY - 1});
    
    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();
        
        if (whiteTiles.count({x, y})) continue;
        if (redTiles.count({x, y})) continue;
        if (x < minX - 1 || x > maxX + 1 || y < minY - 1 || y > maxY + 1) continue;
        
        whiteTiles.insert({x, y});
        
        q.push({x + 1, y});
        q.push({x - 1, y});
        q.push({x, y + 1});
        q.push({x, y - 1});
    }
    
    // Mark remaining as green
    std::set<std::pair<long, long>> greenTiles;
    for (long x = minX; x <= maxX; ++x) {
        for (long y = minY; y <= maxY; ++y) {
            if (!redTiles.count({x, y}) && !whiteTiles.count({x, y})) {
                greenTiles.insert({x, y});
            }
        }
    }
    
    // Check if rectangle contains only red/green tiles
    auto isValidRect = [&](long x1, long y1, long x2, long y2) -> bool {
        long minRectX = std::min(x1, x2);
        long maxRectX = std::max(x1, x2);
        long minRectY = std::min(y1, y2);
        long maxRectY = std::max(y1, y2);
        
        for (long x = minRectX; x <= maxRectX; ++x) {
            for (long y = minRectY; y <= maxRectY; ++y) {
                if (whiteTiles.count({x, y})) return false;
            }
        }
        return true;
    };
    
    // Find largest rectangle with opposite corners on red tiles
    maxArea = 0;
    std::vector<std::pair<long, long>> redVec(redTiles.begin(), redTiles.end());
    
    for (size_t i = 0; i < redVec.size(); ++i) {
        for (size_t j = 0; j < redVec.size(); ++j) {
            if (i == j) continue;
            
            auto [x1, y1] = redVec[i];
            auto [x2, y2] = redVec[j];
            
            // Only axis-aligned rectangles with different x and y
            if (x1 != x2 && y1 != y2) {
                size_t width = std::abs(x2 - x1) + 1;
                size_t height = std::abs(y2 - y1) + 1;
                size_t area = width * height;
                if (area > maxArea) {
                    if (isValidRect(x1, y1, x2, y2)) {
                        maxArea = std::max(maxArea, area);
                    }
                }
            }
        }
    }
    
    std::cout << "Part 2: " << maxArea << std::endl;
    
    return 0;
}