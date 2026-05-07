#include <iostream>
#include <functional>
#include "utils.h"

// make every possible combination of numDigits from digits vector, preserving order
size_t makeNumberFromDigits(const std::vector<size_t>& digits, const int numDigits) {
    size_t maxNumber = 0;
    
    size_t lastNumber = 0;
    size_t lastIndex = 0;
    size_t digitsRemaining = numDigits;
    while (digitsRemaining > 0) {
        for (size_t i = lastIndex; i < digits.size()-digitsRemaining+1; ++i) {
            if (digits[i] > lastNumber) {
                lastNumber = digits[i];
                lastIndex = i;
            }
            if (lastNumber == 9) {
                break;
            }
        }
        maxNumber = maxNumber * 10 + lastNumber;
        lastNumber = 0;
        digitsRemaining--;
        lastIndex = lastIndex + 1;
    }
    
    return maxNumber;
}

int main() {
    std::vector<std::string> lines = utils::readLinesFromFile("input.txt");

    size_t totalCharge = 0;
    // each line represents a bank of batteries with their charge levels
    // each number represents the charge level of one battery in the bank
    for (const auto& line : lines) {
        // find the largest non-consecutive 2 digit charge level in the line
        // i.e. "45362719" -> 79
        // or "811111119" -> 89
        size_t lineMax = 0;
        std::vector<size_t> digits;
        for (char c : line) {
            digits.push_back(c - '0');
        }
        lineMax = makeNumberFromDigits(digits, 2);
        // for (size_t i = 0; i < digits.size()-1; ++i) {
        //     for (size_t j = i+1; j < digits.size(); ++j) {
        //         size_t charge = digits[i] * 10 + digits[j];
        //         if (charge > lineMax) {
        //             lineMax = charge;
        //         }
        //     }
        // }
        // std::cout << line << " -> " << lineMax << std::endl;
        totalCharge += lineMax;
    }
    std::cout << "Part 1: " << totalCharge << std::endl;

    totalCharge = 0;

    for (const auto& line : lines) {
        std::vector<size_t> digits;
        for (char c : line) {
            digits.push_back(c - '0');
        }
        size_t lineMax = makeNumberFromDigits(digits, 12);
        // std::cout << line << " -> " << lineMax << std::endl;
        totalCharge += lineMax;
    }
    std::cout << "Part 2: " << totalCharge << std::endl;
    return 0;
}