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
            // bool inserted = false;
            // for (const auto& pair : rangeMap) {
            //     if (low >= pair.first && low <= pair.second && high > pair.second) {
            //         rangeMap[pair.first] = high;
            //         inserted = true;
            //         break;
            //     } else if (high >= pair.first && high <= pair.second && low < pair.first) {
            //         rangeMap[low] = pair.second;
            //         rangeMap.erase(pair.first);
            //         inserted = true;
            //         break;
            //     }
            // }
            // if (!inserted) {
                rangeMap[low] = high;
            // }
        } else {
            // lines look like "15"
            size_t number = std::stoul(lines[i]);
            for (const auto& pair : rangeMap) {
                if (number >= pair.first && number <= pair.second) {
                    // std::println("Number {} is in range {}-{}", number, pair.first, pair.second);
                    ++counter;
                    break;
                }
            }
        }

    }
   
    std::println("Part 1: {}", counter);
    return 0;
}