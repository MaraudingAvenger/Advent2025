#include "utils.h"
#include "http.h"

#include <iostream>

int main() {

    std::vector<std::string> lines = utils::readLinesFromFile("input.txt");
    const int size = 100;
    int count = 0;
    int currentPosition = 50;
    for (const auto& line : lines) {
        char direction = line[0]; // R or L
        int steps = std::stoi(line.substr(1)); // number of clicks on the dial

        // move the current position and check its value in the pips vector
        // wrap around if necessary
        if (direction == 'R') {
            currentPosition = (currentPosition + steps) % size;
        } else if (direction == 'L') {
            currentPosition = (currentPosition + size - (steps % size)) % size;
        } else {
            throw std::runtime_error("Invalid direction: " + std::string(1, direction));
        }
        if (currentPosition == 0) 
            count++;
    }

    std::cout << "Part one: " << count << std::endl;

    count = 0;
    currentPosition = 50;
    for (const auto& line : lines) {
        char direction = line[0]; // R or L
        int steps = std::stoi(line.substr(1)); // number of clicks on the dial

        // move the current position and check to see if it has crossed 0
        if (direction == 'R') {
            for (int i = 0; i < steps; ++i) {
                currentPosition = (currentPosition + 1) % size;
                if (currentPosition == 0) 
                    count++;
            }
        } else if (direction == 'L') {
            for (int i = 0; i < steps; ++i) {
                currentPosition = (currentPosition + size - 1) % size;
                if (currentPosition == 0) 
                    count++;
            }
        } else {
            throw std::runtime_error("Invalid direction: " + std::string(1, direction));
        }
    }
    std::cout << "Part two: " << count << std::endl;
    return 0;
}