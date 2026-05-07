#include "utils.h"
#include <iostream>
#include <cmath>

// Check if a number is comprised of repeating digit patterns
// e.g., 11, 111, 1212, 123123123 are patterns, but 1234 is not
bool isRepeatingPattern(size_t num) {
    int digitCount = std::floor(std::log10(num)) + 1;
    
    // Try all possible pattern lengths that divide evenly into the total length
    for (int patternLen = 1; patternLen <= digitCount / 2; ++patternLen) {
        if (digitCount % patternLen != 0) {
            continue;
        }
        
        size_t divisor = static_cast<size_t>(std::pow(10, patternLen));
        size_t pattern = num % divisor;
        
        // Check if all segments match the pattern
        bool matches = true;
        size_t temp = num;
        while (temp > 0) {
            if (temp % divisor != pattern) {
                matches = false;
                break;
            }
            temp /= divisor;
        }
        
        if (matches) {
            return true;
        }
    }
    
    return false;
}

int main() {
    size_t total = 0;
    std::string blob = utils::readFileToString("input.txt");
    
    for (auto& line : utils::splitString(blob, ',')) {
        // line is a numerical range in the format "start-end"
        size_t start, end;
        sscanf(line.c_str(), "%zu-%zu", &start, &end);
        
        // iterate over numbers with an even number of digits
        for (size_t num = start; num <= end; ++num) {
            int digitCount = std::floor(std::log10(num)) + 1;
            if (digitCount % 2 == 0) {
                size_t divisor = static_cast<size_t>(std::pow(10, digitCount / 2));
                size_t firstHalf = num / divisor;
                size_t secondHalf = num % divisor;
                if (firstHalf == secondHalf) {
                    total += num;
                }
            }
        }
    }
    std::cout << "Part 1: " << total << std::endl;

    total = 0;
    for (auto& line : utils::splitString(blob, ',')) {
        size_t start, end;
        sscanf(line.c_str(), "%zu-%zu", &start, &end);

        for (size_t num = start; num <= end; ++num) {
            if (isRepeatingPattern(num)) {
                total += num;
            }
        }
    }
    std::cout << "Part 2: " << total << std::endl;

    return 0;
}