#include <algorithm>
#include <bit>
#include <iostream>
#include <cmath>
#include <limits>
#include <map>
#include <ranges>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>

#include "../common/utils.h"

using uchar = unsigned char;
using ushort = unsigned short;

// The result of pressing one particular subset of buttons exactly once each.
struct Combo {
    std::vector<int> delta; // how much each joltage counter increases
    int presses;            // how many buttons are in the subset
};

// Sentinel for "this goal cannot be reached". Kept well below LONG_MAX so that
// `presses + 2 * kImpossible` can never overflow.
constexpr long kImpossible = std::numeric_limits<long>::max() / 4;

// Returns the fewest button presses that raise every counter from 0 to exactly `goal`,
// or kImpossible if no combination of presses can do it.
//
// Why this works:
//   Let x[j] be how many times button j is pressed in some valid solution.
//   Split every count into its odd part and its even part: x[j] = s[j] + 2*y[j], where s[j] is 0 or 1.
//     - The buttons with s[j] == 1 form a subset S, each pressed once.
//     - Pressing S must make every counter's parity (odd/even) match goal's,
//       because the remaining 2*y presses add an even amount to every counter.
//     - What is left, (goal - delta(S)), is all even. Halving it gives a smaller
//       problem whose answer is sum(y), costing 2*sum(y) presses in the original.
//   So: total = |S| + 2 * minPresses((goal - delta(S)) / 2). We try every S with the right
//   parity and keep the cheapest. Each recursion roughly halves the targets, so the depth
//   is only about log2(max target) levels.
long minPresses(const std::vector<int>& goal,
                const std::map<std::vector<int>, std::vector<Combo>>& combosByParity,
                std::map<std::vector<int>, long>& memo) {
    // Base case: every counter is already at zero, so no presses are needed.
    if (std::ranges::all_of(goal, [](int v) { return v == 0; })) return 0;
    // Different subset choices often lead to the same smaller goal; reuse earlier answers.
    if (auto it = memo.find(goal); it != memo.end()) return it->second;

    // Odd/even pattern of the goal (1 = odd, 0 = even). Only subsets that produce this exact
    // pattern can leave an all-even remainder.
    auto parity = goal | std::views::transform([](int v) { return v & 1; })
                       | std::ranges::to<std::vector<int>>();
    long best = kImpossible;
    if (auto it = combosByParity.find(parity); it != combosByParity.end()) {
        for (const auto& combo : it->second) {
            // Subtract this subset's effect and halve what remains. Skip the subset if it
            // overshoots any counter, since presses can only ever increase counters.
            std::vector<int> half(goal.size());
            bool valid = true;
            for (size_t i = 0; i < goal.size(); ++i) {
                int remaining = goal[i] - combo.delta[i];
                if (remaining < 0) { valid = false; break; }
                half[i] = remaining / 2; // exact: parities matched, so remaining is even
            }
            if (!valid) continue;

            // Solve the halved problem; every press there counts twice here.
            long sub = minPresses(half, combosByParity, memo);
            if (sub < kImpossible) best = std::min(best, combo.presses + 2 * sub);
        }
    }
    // If no subset matched the parity (or all overshot), best stays kImpossible.
    memo.emplace(goal, best);
    return best;
}

int main() {
    size_t stepTotal = 0;
    size_t joltageTotal = 0;
    for (const auto& line : utils::readLinesFromFile(utils::getInputFilePath("inputs/day10.txt"))) {
        // Example line:  [.#.#] (0,2,3) (1,3) {11,10,11,21}
        //   [.#.#]          target light pattern ('#' = on, '.' = off)
        //   (0,2,3) (1,3)   buttons; each lists the indices it affects
        //   {11,10,11,21}   joltage targets for Part 2, one per counter
        std::vector<std::string> parts = utils::splitString(line);
        std::string buttonString = utils::trim(parts[0], "[]");
        std::string jolts = utils::trim(parts.back(), "{}");
        parts.pop_back();
        // Everything between the [lights] and the {joltages} is a button pattern.
        std::vector<std::string> patternStrings = parts 
                | std::views::drop(1) 
                | std::views::take(parts.size() - 1) 
                | std::ranges::to<std::vector<std::string>>();
        
        // Convert the light diagram into booleans: true where the light must end up on.
        std::vector<bool> target(buttonString.size(), false);
        for (size_t i = 0; i < buttonString.size(); ++i) {
            if (buttonString[i] == '#') target[i] = true;
        }

        // Parse each "(0,2,3)" into a list of indices. Part 1 treats these as lights to toggle;
        // Part 2 treats them as joltage counters to increment.
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
        // All lights start off. Pressing a button toggles every light it lists.
        // Find the fewest presses that produce the `target` light pattern.
        //
        // Breadth-first search over light states: BFS explores states in order of press count,
        // so the first time we reach `target` is guaranteed to use the fewest presses.
        // The state space is at most 2^(number of lights), which stays small.
        std::vector<bool> initial(buttonString.size(), false);
        std::set<std::vector<bool>> visited; // states already queued, so none is explored twice
        std::queue<std::pair<std::vector<bool>, int>> q; // (light state, presses so far)
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

            // Try pressing each button once from this state.
            for (const auto& pattern : patterns) {
                std::vector<bool> next = current;
                for (uchar idx : pattern) {
                    next[idx] = !next[idx]; // toggle the light
                }
                if (visited.find(next) == visited.end()) {
                    visited.insert(next);
                    q.push({next, steps + 1});
                }
            }
        }
        stepTotal += minSteps;

        
        // Part 2
        // Now each button adds 1 to every joltage counter it lists, and all counters start at 0.
        // Find the fewest presses that bring every counter to exactly its target value.
        //
        // A search over counter states (as in Part 1) blows up: with targets in the hundreds
        // across ~10 counters there are far too many states. Since button order doesn't matter,
        // only how many times each one is pressed, this is really an integer equation system:
        //   minimize sum(x)  subject to  A * x = joltTarget,  x >= 0 and integer,
        // where A[i][j] = 1 if button j increments counter i. minPresses() solves it by
        // repeatedly fixing the odd/even part of the counts and halving the problem.
        std::vector<ushort> joltTarget;
        for (const auto& token : utils::splitString(jolts, ',')) {
            joltTarget.push_back(static_cast<ushort>(std::stoi(token)));
        }
        
        // Precompute the effect of pressing every subset of buttons exactly once
        // (at most 2^13 = 8192 subsets here). Group them by the odd/even pattern they produce,
        // so minPresses() can jump straight to the subsets that match a goal's parity.
        std::map<std::vector<int>, std::vector<Combo>> combosByParity;
        for (unsigned mask = 0; mask < (1u << patterns.size()); ++mask) {
            // Bit b of `mask` set means button b is in this subset; popcount = presses used.
            Combo combo{std::vector<int>(joltTarget.size(), 0), std::popcount(mask)};
            for (size_t b = 0; b < patterns.size(); ++b) {
                if (mask & (1u << b)) {
                    for (uchar idx : patterns[b]) {
                        ++combo.delta[idx];
                    }
                }
            }
            auto parity = combo.delta | std::views::transform([](int v) { return v & 1; })
                                      | std::ranges::to<std::vector<int>>();
            combosByParity[std::move(parity)].push_back(std::move(combo));
        }

        // Solve for this machine. The memo is per-line because combosByParity depends on its buttons.
        std::vector<int> joltGoal(joltTarget.begin(), joltTarget.end());
        std::map<std::vector<int>, long> memo;
        long minJoltSteps = minPresses(joltGoal, combosByParity, memo);
        if (minJoltSteps >= kImpossible) {
            throw std::runtime_error("No button combination reaches joltage target: " + line);
        }
        joltageTotal += minJoltSteps;
    }
    std::cout << std::endl;
    std::cout << "Part 1: " << stepTotal << std::endl;
    std::cout << "Part 2: " << joltageTotal << std::endl;
}