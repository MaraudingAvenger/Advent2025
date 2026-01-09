#include <iostream>
#include <vector>
#include <print>
#include <unordered_map>
#include "utils.h"

int main() {
    int counter = 0;
    std::vector<std::string> lines = utils::readLinesFromFile("input.txt");
    bool foundEmptyLine = false;
    std::unordered_map<size_t, size_t> rangeMap;
    std::vector<std::pair<size_t, size_t>> ranges;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].empty()) {
            foundEmptyLine = true;
            continue;
        }

        if (!foundEmptyLine) {
            // lines look like "10-20" meaning the range from 10 to 20 inclusive
            std::vector<std::string> parts = utils::splitString(lines[i], "-");
            size_t low = std::stoul(parts[0]);
            size_t high = std::stoul(parts[1]);
            std::pair<size_t, size_t> range = {low, high};
            ranges.push_back(range);
        } else {
            // lines look like "15"
            size_t number = std::stoul(lines[i]);
            for (const auto& pair : ranges) {
                if (number >= pair.first && number <= pair.second) {
                    ++counter;
                    break;
                }
            }
        }

    }
    std::println("Part 1: {}", counter);

    // merge overlapping ranges
    std::sort(ranges.begin(), ranges.end());
    std::vector<std::pair<size_t, size_t>> mergedRanges;
    for (const auto& range : ranges) {
        if (mergedRanges.empty() || mergedRanges.back().second < range.first) {
            mergedRanges.push_back(range);
        } else {
            mergedRanges.back().second = std::max(mergedRanges.back().second, range.second);
        }
    }

    size_t totalCovered = 0;
    for (const auto& range : mergedRanges) {
        totalCovered += (range.second - range.first + 1);
    }

    std::println("Part 2: {}", totalCovered);
    return 0;
}