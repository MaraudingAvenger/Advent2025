#include <iostream>
#include <vector>
#include <string>
#include <print>
#include <numeric>
#include <ranges>
#include <charconv>
#include <algorithm>

#include "../common/utils.h"

namespace {

// Parse a string to size_t using std::from_chars (C++17, efficient in C++23)
size_t parseNumber(std::string_view sv) {
    size_t result = 0;
    auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), result);
    if (ec != std::errc{}) {
        throw std::invalid_argument("Invalid number: " + std::string(sv));
    }
    return result;
}

// Apply an operator to a list of operands
size_t applyOp(char op, std::span<const size_t> operands) {
    if (op == '+') {
        return std::accumulate(operands.begin(), operands.end(), size_t{0});
    } else if (op == '*') {
        return std::accumulate(operands.begin(), operands.end(), size_t{1}, std::multiplies<>{});
    }
    throw std::invalid_argument("Unknown operator: " + std::string(1, op));
}

} // namespace

int main() {
    auto lines = utils::readLinesFromFile(utils::getInputFilePath("inputs/day6.txt"));
    
    // Part 1: Split each line by whitespace, then transpose
    auto dataLines = std::vector<std::string>(lines.begin(), lines.end() - 1);
    auto opLine = utils::splitString(lines.back());
    
    // Split each data line by whitespace first
    std::vector<std::vector<std::string>> splitLines;
    splitLines.reserve(dataLines.size());
    for (const auto& line : dataLines) {
        splitLines.push_back(utils::splitString(line));
    }
    
    // Transpose the split lines (not characters)
    if (splitLines.empty()) {
        std::println("Part 1: 0");
        return 0;
    }
    
    size_t numCols = splitLines[0].size();
    std::vector<std::vector<size_t>> columns(numCols);
    for (const auto& splitLine : splitLines) {
        for (size_t i = 0; i < splitLine.size(); ++i) {
            columns[i].push_back(parseNumber(splitLine[i]));
        }
    }
    
    size_t total = 0;
    for (size_t i = 0; i < columns.size(); ++i) {
        char op = opLine[i][0];
        total += applyOp(op, std::span(columns[i]));
    }
    
    std::println("Part 1: {}", total);

    // Part 2: Read vertically, apply operators from right to left
    auto fileStr = utils::readFileToString(utils::getInputFilePath("inputs/day6.txt"));
    auto allLines = utils::splitString(fileStr, "\n");
    
    // Strip trailing empty lines
    while (!allLines.empty() && allLines.back().empty()) {
        allLines.pop_back();
    }
    
    auto dataLines2 = std::vector<std::string>(allLines.begin(), allLines.end() - 1);
    std::string_view operatorLine = allLines.back();
    auto symbols = utils::splitString(operatorLine);
    
    size_t total2 = 0;
    std::vector<size_t> operands;
    
    // Iterate from right to left through the operator line
    for (std::size_t i = operatorLine.size(); i > 0; --i) {
        // Collect digits vertically at column i-1
        std::string nStr;
        for (const auto& line : dataLines2) {
            if (i - 1 < line.size() && std::isdigit(static_cast<unsigned char>(line[i - 1]))) {
                nStr += line[i - 1];
            }
        }
        
        // Trim whitespace
        nStr = utils::trim(nStr);
        
        if (!nStr.empty()) {
            // Found a number, add to operands
            operands.push_back(parseNumber(nStr));
        } else {
            // Found an operator, apply it
            if (!symbols.empty()) {
                auto op = symbols.back();
                symbols.pop_back();
                total2 += applyOp(op[0], std::span(operands));
                operands.clear();
            }
        }
    }
    
    // Handle final operation if there are remaining operands
    if (!operands.empty()) {
        if (!symbols.empty()) {
            auto op = symbols.back();
            symbols.pop_back();
            total2 += applyOp(op[0], std::span(operands));
        }
    }
    
    std::println("Part 2: {}", total2);
} 