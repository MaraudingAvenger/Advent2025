#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include "utils.h"


using Point = std::pair<long, long>;

// Computes inclusive axis-aligned rectangle area from two opposite corners.
// +1 is used because puzzle coordinates represent tile positions (inclusive bounds).
size_t rectArea(Point topLeft, Point bottomRight) {
    size_t width = std::abs(bottomRight.first - topLeft.first) + 1;
    size_t height = std::abs(bottomRight.second - topLeft.second) + 1;
    return width * height;
}

struct CompressedPolygon {
    // Sorted unique x/y coordinates from vertex list (coordinate compression axes).
    std::vector<long> xs;
    std::vector<long> ys;
    // insideCells[row][col] = 1 when compressed cell is strictly inside polygon.
    std::vector<std::vector<uint8_t>> insideCells;   // [row y-interval][col x-interval]
    // hasVertex[y][x] = 1 when original vertex exists at compressed grid intersection.
    std::vector<std::vector<uint8_t>> hasVertex;     // [y-index][x-index]
};

// Builds compressed polygon representation used by Part 2.
// Strategy:
// 1) Coordinate-compress x/y from vertices.
// 2) Mark which compressed intersections are original vertices.
// 3) Fill compressed cells using scanline parity (even/odd intersections).
CompressedPolygon preparePolygonData(const std::vector<Point>& coordinates) {
    CompressedPolygon data;

    // Collect all x/y values from vertices.
    for (const auto& [x, y] : coordinates) {
        data.xs.push_back(x);
        data.ys.push_back(y);
    }

    // Deduplicate + sort for coordinate compression.
    std::sort(data.xs.begin(), data.xs.end());
    data.xs.erase(std::unique(data.xs.begin(), data.xs.end()), data.xs.end());
    std::sort(data.ys.begin(), data.ys.end());
    data.ys.erase(std::unique(data.ys.begin(), data.ys.end()), data.ys.end());

    // Maps from world coordinate -> compressed index.
    std::map<long, int> xIndex;
    std::map<long, int> yIndex;
    for (int i = 0; i < static_cast<int>(data.xs.size()); ++i) {
        xIndex[data.xs[i]] = i;
    }
    for (int i = 0; i < static_cast<int>(data.ys.size()); ++i) {
        yIndex[data.ys[i]] = i;
    }

    // Mark every original vertex on the compressed lattice.
    data.hasVertex.assign(data.ys.size(), std::vector<uint8_t>(data.xs.size(), 0));
    for (const auto& [x, y] : coordinates) {
        data.hasVertex[yIndex[y]][xIndex[x]] = 1;
    }

    // Need at least one compressed cell in both dimensions.
    if (data.xs.size() < 2 || data.ys.size() < 2) {
        return data;
    }

    const int rows = static_cast<int>(data.ys.size()) - 1;
    const int cols = static_cast<int>(data.xs.size()) - 1;
    data.insideCells.assign(rows, std::vector<uint8_t>(cols, 0));

    // For each compressed y-band, cast a horizontal scanline through its midpoint.
    // Intersections with vertical polygon edges define inside intervals (parity fill).
    for (int row = 0; row < rows; ++row) {
        long double yMid = (static_cast<long double>(data.ys[row]) + static_cast<long double>(data.ys[row + 1])) * 0.5L;

        std::vector<long> intersections;
        intersections.reserve(coordinates.size());

        // Build x-intersections against vertical edges only.
        for (size_t i = 0; i < coordinates.size(); ++i) {
            const auto& p1 = coordinates[i];
            const auto& p2 = coordinates[(i + 1) % coordinates.size()];

            if (p1.first == p2.first) {
                long yMin = std::min(p1.second, p2.second);
                long yMax = std::max(p1.second, p2.second);
                // Half-open y-range avoids double counting at shared vertices.
                if (yMid >= static_cast<long double>(yMin) && yMid < static_cast<long double>(yMax)) {
                    intersections.push_back(p1.first);
                }
            }
        }

        std::sort(intersections.begin(), intersections.end());
        if (intersections.size() < 2) {
            continue;
        }

        // Step through compressed x-cells and mark midpoint-inside intervals.
        size_t interval = 0;
        for (int col = 0; col < cols; ++col) {
            long double xMid = (static_cast<long double>(data.xs[col]) + static_cast<long double>(data.xs[col + 1])) * 0.5L;

            while (interval + 1 < intersections.size() && xMid >= static_cast<long double>(intersections[interval + 1])) {
                interval += 2;
            }

            if (interval + 1 < intersections.size()) {
                long left = intersections[interval];
                long right = intersections[interval + 1];
                if (xMid > static_cast<long double>(left) && xMid < static_cast<long double>(right)) {
                    data.insideCells[row][col] = 1;
                }
            }
        }
    }

    return data;
}

// 2D prefix-sum for O(1) rectangular area queries over insideCells.
std::vector<std::vector<int>> buildPrefix(const std::vector<std::vector<uint8_t>>& grid) {
    if (grid.empty() || grid[0].empty()) {
        return {{0}};
    }

    int rows = static_cast<int>(grid.size());
    int cols = static_cast<int>(grid[0].size());
    std::vector<std::vector<int>> prefix(rows + 1, std::vector<int>(cols + 1, 0));

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            prefix[r + 1][c + 1] = grid[r][c]
                + prefix[r][c + 1]
                + prefix[r + 1][c]
                - prefix[r][c];
        }
    }

    return prefix;
}

// Returns sum over inclusive rectangle [r1..r2] x [c1..c2] in insideCells.
int sumRect(const std::vector<std::vector<int>>& prefix, int r1, int c1, int r2, int c2) {
    if (r1 > r2 || c1 > c2) {
        return 0;
    }
    return prefix[r2 + 1][c2 + 1]
         - prefix[r1][c2 + 1]
         - prefix[r2 + 1][c1]
         + prefix[r1][c1];
}

// Part 2 solver.
// Finds largest axis-aligned rectangle that:
// - is fully inside the filled orthogonal polygon, and
// - has opposite corners that are original polygon vertices.
//
// Efficiency notes:
// - Uses compressed coordinates, so work scales with unique x/y counts.
// - For each (bottomY, topY) pair, tests valid interior x-runs using prefix sums.
// - Per run, picks widest diagonal based on available vertices at top/bottom rows.
size_t solvePart2(const std::vector<Point>& coordinates) {
    // Quick reject for impossible rectangles.
    if (coordinates.size() < 4) {
        return 0;
    }

    CompressedPolygon data = preparePolygonData(coordinates);
    if (data.xs.size() < 2 || data.ys.size() < 2 || data.insideCells.empty() || data.insideCells[0].empty()) {
        return 0;
    }

    const int yCount = static_cast<int>(data.ys.size());
    const int xCount = static_cast<int>(data.xs.size());
    const int cols = xCount - 1;

    // Prefix grid enables constant-time check that a column strip is fully inside.
    std::vector<std::vector<int>> prefix = buildPrefix(data.insideCells);
    size_t maxArea = 0;

    // Enumerate candidate rectangle heights by choosing bottom/top compressed y indices.
    for (int yBottom = 0; yBottom < yCount; ++yBottom) {
        for (int yTop = yBottom + 1; yTop < yCount; ++yTop) {
            size_t height = static_cast<size_t>(std::abs(data.ys[yTop] - data.ys[yBottom])) + 1;

            // Upper-bound pruning for current height.
            size_t maxPossibleWithHeight = height * (static_cast<size_t>(std::abs(data.xs.back() - data.xs.front())) + 1);
            if (maxPossibleWithHeight <= maxArea) {
                continue;
            }

            const std::vector<uint8_t>& bottomVertices = data.hasVertex[yBottom];
            const std::vector<uint8_t>& topVertices = data.hasVertex[yTop];

            // Mark compressed x-columns whose full vertical strip is inside polygon.
            std::vector<uint8_t> goodCellCol(cols, 0);
            for (int col = 0; col < cols; ++col) {
                int insideCount = sumRect(prefix, yBottom, col, yTop - 1, col);
                goodCellCol[col] = (insideCount == (yTop - yBottom));
            }

            // Maximal contiguous runs of good columns are candidate rectangle x-ranges.
            int col = 0;
            while (col < cols) {
                while (col < cols && !goodCellCol[col]) {
                    ++col;
                }
                if (col >= cols) {
                    break;
                }

                int startCol = col;
                while (col < cols && goodCellCol[col]) {
                    ++col;
                }
                int endCol = col - 1;

                if (startCol <= endCol) {
                    const int leftX = startCol;
                    const int rightX = endCol + 1;

                    // Find outermost vertices on both candidate y edges in this run.
                    int firstBottom = -1;
                    int lastBottom = -1;
                    int firstTop = -1;
                    int lastTop = -1;

                    for (int x = leftX; x <= rightX; ++x) {
                        if (bottomVertices[x]) {
                            if (firstBottom == -1) {
                                firstBottom = x;
                            }
                            lastBottom = x;
                        }
                        if (topVertices[x]) {
                            if (firstTop == -1) {
                                firstTop = x;
                            }
                            lastTop = x;
                        }
                    }

                    size_t bestRunWidth = 0;

                    // Diagonal option A: bottom-left with top-right.
                    if (firstBottom != -1 && lastTop != -1 && firstBottom < lastTop) {
                        size_t w = static_cast<size_t>(std::abs(data.xs[lastTop] - data.xs[firstBottom])) + 1;
                        if (w > bestRunWidth) {
                            bestRunWidth = w;
                        }
                    }

                    // Diagonal option B: top-left with bottom-right.
                    if (firstTop != -1 && lastBottom != -1 && firstTop < lastBottom) {
                        size_t w = static_cast<size_t>(std::abs(data.xs[lastBottom] - data.xs[firstTop])) + 1;
                        if (w > bestRunWidth) {
                            bestRunWidth = w;
                        }
                    }

                    // Update global best area.
                    if (bestRunWidth > 0) {
                        size_t area = bestRunWidth * height;
                        if (area > maxArea) {
                            maxArea = area;
                        }
                    }
                }
            }
        }
    }

    return maxArea;
}

int main() {
    // Input format: "x,y" per line.
    std::vector<Point> coordinates;
    std::ifstream file("input.txt");
    long x, y;
    char comma;
    while (file >> x >> comma >> y) {
        coordinates.emplace_back(x, y);
    }
    file.close();

    // Part 1: original logic - max inclusive rectangle area from any two points.
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

    // Part 2: fill polygon induced by vertex order (including closing edge), then
    // find largest inside rectangle whose opposite corners are original vertices.
    maxArea = solvePart2(coordinates);

    std::cout << "Part 2: " << maxArea << std::endl;
    
    return 0;
}