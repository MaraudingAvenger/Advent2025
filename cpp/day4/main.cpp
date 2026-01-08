#include <iostream>
#include <vector>
#include <algorithm>
#include "utils.h"

size_t convertToCoords(size_t row, size_t col, size_t lineLen) {
    return row * lineLen + col;
}

size_t countSurroundingMarked(const std::vector<unsigned short>& marked, size_t row, size_t col, size_t lineLen) {
    size_t count = 0;
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if ((dr == 0 && dc == 0) || 
                (row == 0 && dr == -1) || 
                (col == 0 && dc == -1) || 
                (row == marked.size() / lineLen - 1 && dr == 1) || 
                (col == lineLen - 1 && dc == 1)) 
                continue;
            size_t newRow = row + dr;
            size_t newCol = col + dc;
            if (marked[convertToCoords(newRow, newCol, lineLen)]) {
                ++count;
            }
        }
    }
    return count;
}

int main() {
    std::vector<std::string> lines = utils::readLinesFromFile("input.txt");
    size_t lineLen = lines[0].size();
    std::vector<unsigned short> marked(lines.size() * lineLen, 0);
    for (size_t row = 0; row < lines.size(); ++row) {
        for (size_t col = 0; col < lineLen; ++col) {
            if (lines[row][col] == '@') {
                marked[convertToCoords(row, col, lineLen)] = 1;
            }
        }
    }

    size_t total = 0;
    for (size_t row = 0; row < lines.size(); ++row) {
        for (size_t col = 0; col < lineLen; ++col) {
            // std::cout << countSurroundingMarked(marked, row, col, lineLen) << " ";
            if (marked[convertToCoords(row, col, lineLen)]) {
                if (countSurroundingMarked(marked, row, col, lineLen) < 4) {
                    ++total;
                }
            }
        }
        // std::cout << '\t' << lines[row];
        // std::cout << std::endl;
    }
    std::cout << "Part 1: " << total << std::endl;

    total = 0;
    size_t lastTotal = 123456789;
    while (total != lastTotal) {
        lastTotal = total;
        for (size_t row = 0; row < lines.size(); ++row) {
            for (size_t col = 0; col < lineLen; ++col) {
                if (marked[convertToCoords(row, col, lineLen)]) {
                    if (countSurroundingMarked(marked, row, col, lineLen) < 4) {
                        ++total;
                        marked[convertToCoords(row, col, lineLen)] = 0;
                    }
                }
            }
        }
    }
    std::cout << "Part 2: " << total << std::endl;
    return 0;
}