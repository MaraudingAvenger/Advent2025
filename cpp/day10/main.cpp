#include <iostream>
#include <cmath>
#include <limits>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>

#include "../common/utils.h"

using uchar = unsigned char;
using ushort = unsigned short;

int main() {
    size_t stepTotal = 0;
    size_t joltageTotal = 0;
    for (const auto& line : utils::readLinesFromFile(utils::getInputFilePath("inputs/day10.txt"))) {
        // [.#.#] (0,2,3) (1,3) {11,10,11,21}
        // [buttons] (flip patterns) (...) {jolt values}
        std::vector<std::string> parts = utils::splitString(line);
        std::string buttonString = utils::trim(parts[0], "[]");
        std::string jolts = utils::trim(parts.back(), "{}");
        parts.pop_back();
        std::vector<std::string> patternStrings = parts 
                | std::views::drop(1) 
                | std::views::take(parts.size() - 1) 
                | std::ranges::to<std::vector<std::string>>();
        
        std::vector<bool> target(buttonString.size(), false);
        for (size_t i = 0; i < buttonString.size(); ++i) {
            if (buttonString[i] == '#') target[i] = true;
        }

        // parse patterns into 2D vector of switch indices to flip
        std::vector<std::vector<uchar>> patterns;
        for (const auto& patternString : patternStrings) {
            std::vector<std::string> tokens = utils::splitString(
                utils::trim(patternString, "()"), ',');
            std::vector<uchar> pattern;
            for (const auto& token : tokens) {
                pattern.push_back(static_cast<uchar>(std::stoi(token)));
            }
            patterns.push_back(std::move(pattern));
        }
        
        // Part 1
        // target is the final pattern the switches need to be in
        // patterns are the available flip operations that can be applied in any order
        // we want to find the minimum number of patterns needed to reach the target from the initial all-off state
        std::vector<bool> initial(buttonString.size(), false);
        std::set<std::vector<bool>> visited;
        std::queue<std::pair<std::vector<bool>, int>> q; // (current state, steps)
        q.push({initial, 0});
        visited.insert(initial);
        int minSteps = -1;

        while (!q.empty()) {
            auto [current, steps] = q.front();
            q.pop();

            if (current == target) {
                minSteps = steps;
                break;
            }

            for (const auto& pattern : patterns) {
                std::vector<bool> next = current;
                for (uchar idx : pattern) {
                    next[idx] = !next[idx]; // flip the switch
                }
                if (visited.find(next) == visited.end()) {
                    visited.insert(next);
                    q.push({next, steps + 1});
                }
            }
        }
        stepTotal += minSteps;

        
        // Part 2
        
        // Now we treat the button patterns as joltage incrementers.
        // Each time we press a button pattern, we increment the joltage at those indices by 1. 
        // We want to find the minimum number of button presses needed to reach the target joltage 
        // configuration from an initial all-zero state. On some of the lines, the jolt target might be
        // very large (>200), so we need to be careful about excess branching, and make sure to 
        // abort any search path that exceeds the target joltage or minimum steps found so far.
        
        std::vector<ushort> joltTarget;
        for (const auto& token : utils::splitString(jolts, ',')) {
            joltTarget.push_back(static_cast<ushort>(std::stoi(token)));
        }
        std::vector<ushort> initialJolts(joltTarget.size(), 0);
        std::set<std::vector<ushort>> visitedJolts;
        std::queue<std::pair<std::vector<ushort>, int>> q2; // (current jolts, steps)
        q2.push({initialJolts, 0});
        visitedJolts.insert(initialJolts);
        int minJoltSteps = -1;
        std::cout << "{ ";
        for (ushort j : joltTarget) {
            std::cout << j << ' ';
        }
        std::cout << "} -> ";

        while (!q2.empty()) {
            auto [currentJolts, steps] = q2.front();
            q2.pop();

            if (currentJolts == joltTarget) {
                minJoltSteps = steps;
                break;
            }

            for (const auto& pattern : patterns) {
                std::vector<ushort> nextJolts = currentJolts;
                for (uchar idx : pattern) {
                    nextJolts[idx] += 1; // increment the joltage
                }
                // Check if we can prune this path
                bool exceedsTarget = false;
                for (size_t i = 0; i < nextJolts.size(); ++i) {
                    if (nextJolts[i] > joltTarget[i]) {
                        exceedsTarget = true;
                        break;
                    }
                }
                if (exceedsTarget) continue;

                if (visitedJolts.find(nextJolts) == visitedJolts.end()) {
                    visitedJolts.insert(nextJolts);
                    q2.push({nextJolts, steps + 1});
                }
            }
        }
        std::cout << minJoltSteps << '\n';
        joltageTotal += minJoltSteps;
    }
    std::cout << std::endl;
    std::cout << "Part 1: " << stepTotal << std::endl;
    std::cout << "Part 2: " << joltageTotal << std::endl;
}