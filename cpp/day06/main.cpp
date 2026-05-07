#include <iostream>
#include <vector>
#include <sstream>
#include <print>

#include "utils.h"


int main() {
    std::vector<std::string> lines = utils::readLinesFromFile("input.txt");
    // transpose lines into columns
    std::vector<std::vector<std::string>> problems;
    bool first = true;
    for (const auto& line : lines) {
        std::vector<std::string> parts = utils::splitString(line);
        if (first) {
            problems.resize(parts.size());
            first = false;
        }
        for (size_t i = 0; i < parts.size(); ++i) {
            problems[i].push_back(parts[i]);
        }
    }

    size_t total = 0;

    for (const auto& problem : problems) {
        // last element is the operator
        const std::string& op = problem.back();
        std::vector<std::string> operands(problem.begin(), problem.end() - 1);
        switch (op[0]) {
            case '+': {
                size_t sum = 0;
                for (const auto& operand : operands) {
                    sum += std::stoul(operand);
                }
                // std::cout << sum << "\n";
                total += sum;
                break;
            }
            case '*': {
                size_t product = 1;
                for (const auto& operand : operands) {
                    product *= std::stoul(operand);
                }
                // std::cout << product << "\n";
                total += product;
                break;
            }
            default:
                std::cerr << "Unknown operator: " << op << "\n";
                break;
        }
    }

    std::println("Part 1: {}", total);

    
    size_t total2 = 0;

    std::string fileStr = utils::readFileToString("input.txt");
    lines = utils::splitString(fileStr, "\n");
    std::string opLine = lines.back();
    while (utils::trim(opLine).empty()) {
        lines.pop_back();
        opLine = lines.back();
    }
    std::vector<std::string> symbols = utils::splitString(utils::trim(opLine));
    std::vector<size_t> operands;
    for (int i = opLine.length()-1; i >= 0; --i) {
        std::string nStr = "";
        for (const auto& line : std::vector<std::string>(lines.begin(), lines.end() -1)) {
            if ((size_t) i < line.size() && std::isdigit(line[i])) {
                nStr.append(1, line[i]);
            }
        }
        
        nStr = utils::trim(nStr);
        
        if (!nStr.empty()) {
            size_t n = std::stoul(nStr);
            operands.push_back(n);
            continue;
        }
        
        std::string op = symbols.back();
        symbols.pop_back();
        switch (op[0]) {
            case '+': {
                size_t sum = 0;
                for (size_t& operand : operands) {
                    sum += operand;
                }
                operands.clear();
                total2 += sum;
                break;
            }
            case '*': {
                size_t product = 1;
                for (size_t& operand : operands) {
                    product *= operand;
                }
                operands.clear();
                total2 += product;
                break;
            }
            default: {
                std::cerr << "Unknown operator: " << op << "\n";
                break;
            }
            continue;
        }
    }
    // final operation
    std::string op = symbols.back();
    symbols.pop_back();
    switch (op[0]) {
        case '+': {
            size_t sum = 0;
            for (size_t& operand : operands) {
                sum += operand;
            }
            operands.clear();
            total2 += sum;
            break;
        }
        case '*': {
            size_t product = 1;
            for (size_t& operand : operands) {
                product *= operand;
            }
            operands.clear();
            total2 += product;
            break;
        }
        default: {
            std::cerr << "Unknown operator: " << op << "\n";
            break;
        }
    }
    std::println("Part 2: {}", total2);
} 