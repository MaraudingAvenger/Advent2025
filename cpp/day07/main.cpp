#include <iostream>
#include <numeric>
#include <ranges>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "../common/utils.h"

void printIndices(const std::unordered_map<size_t, size_t>& map) {
    std::cout << "{ ";
    for (const auto& [key, value] : map) {
        std::cout << key << ": " << value << ", ";
    }
    std::cout << " }" << std::endl;
}

int main() {
    std::vector<std::string> lines = utils::readLinesFromFile(utils::getInputFilePath("inputs/day7.txt"));
    std::unordered_set<size_t> beamIndices;
    beamIndices.insert(lines.front().find('S', 0));
    size_t splits = 0;
    for (const auto& line : lines) {
        std::unordered_set<size_t> newBeamIndices;
        for (size_t index : beamIndices) {
            if (line[index] == '^') {
                splits++;
                newBeamIndices.insert(index-1);
                newBeamIndices.insert(index+1);
            } else {
                newBeamIndices.insert(index);
            }
        }
        beamIndices = std::move(newBeamIndices);
        newBeamIndices.clear();
    }
    std::cout << "Part 1: " << splits << std::endl;

    std::unordered_map<size_t, size_t> pathCount;
    pathCount[lines.front().find('S', 0)] = 1;
    for (const auto& line : lines) {
        std::unordered_map<size_t, size_t> newPathCount;
        std::vector<size_t> toRemove;
        for (size_t index : pathCount | std::views::keys) {
            if (line[index] == '^') {
                toRemove.push_back(index);
                size_t leftIndex = index - 1;
                size_t rightIndex = index + 1;
                size_t value = pathCount[index];
                newPathCount[leftIndex] += value;
                newPathCount[rightIndex] += value;
            } else {
                newPathCount[index] += pathCount[index];
            }
        }
        for (size_t index : toRemove) {
            pathCount.erase(index);
        }
        toRemove.clear();

        for (const auto& [key, val] : newPathCount) {
            if (pathCount.find(key) != pathCount.end()) {
                pathCount[key] = val;
            } else {
                pathCount[key] += val;
            }
        }
        newPathCount.clear();

        // for (size_t i = 0; i < line.size(); i++) {
        //     if (pathCount.find(i) != pathCount.end()) {
        //         std::cout << pathCount[i]%10;
        //     } else {
        //         std::cout << line[i];
        //     }
        // }
        // std::cout << std::endl;
    }
    std::cout << "Part 2: " << std::accumulate(
        pathCount.begin(), pathCount.end(), 0ULL,
        [](size_t sum, const std::pair<size_t, size_t>& p) {
            return sum + p.second;
        }) << std::endl;
    return 0;
}