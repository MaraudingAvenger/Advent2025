#include <vector>
#include <string>
#include <iostream>
#include <print>

#include "../common/utils.h"

class Shape {
public:
    Shape(std::string s) {
        int x = 0, y = 0;
        for (char c : s) {
            if (c == '\n') {
                y++;
                x = 0;
                continue;
            }
            if (c == '#') {
                coords.push_back({x, y});
            }
            x++;
        }
    }
    Shape(std::vector<std::pair<int,int>> c) : coords(std::move(c)) {}
    
    std::vector<std::pair<int,int>> coords;

    Shape rotate(int degrees=90) const {
        std::vector<std::pair<int,int>> rotated;

        switch (degrees / 90) {
            case 1:
                for (auto [x, y] : coords) {
                    rotated.push_back({-y, x});
                }
                break;
            case 2:
                for (auto [x, y] : coords) {
                    rotated.push_back({-x, -y});
                }
                break;
            case 3:
                for (auto [x, y] : coords) {
                    rotated.push_back({y, -x});
                }
                break;
            default:
                throw std::invalid_argument("Unsupported rotation angle");
        }
        

        // normalize rotated points to 0,0
        int minX = std::min_element(rotated.begin(), rotated.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
        })->first;
        int minY = std::min_element(rotated.begin(), rotated.end(), [](const auto& a, const auto& b) {
            return a.second < b.second;
        })->second;
        for (auto& [x, y] : rotated) {
            x -= minX;
            y -= minY;
        }
        return Shape(rotated);

    }

    Shape reflect(bool horizontal=true) const {
        std::vector<std::pair<int,int>> reflected;
        if (horizontal) {
            int maxX = std::max_element(coords.begin(), coords.end(), [](const auto& a, const auto& b) {
                return a.first < b.first;
            })->first;
            for (auto [x, y] : coords) {
                reflected.push_back({maxX - x, y});
            }
        } else {
            int maxY = std::max_element(coords.begin(), coords.end(), [](const auto& a, const auto& b) {
                return a.second < b.second;
            })->second;
            for (auto [x, y] : coords) {
                reflected.push_back({x, maxY - y});
            }
        }
        return Shape(reflected);
    }

    std::string toString() const {
        int maxX = 0, maxY = 0;
        for (auto [x, y] : coords) {
            if (x > maxX) maxX = x;
            if (y > maxY) maxY = y;
        }
        std::vector<std::string> grid(maxY + 1, std::string(maxX + 1, '.'));
        for (auto [x, y] : coords) {
            grid[y][x] = '#';
        }
        std::string result;
        for (const auto& row : grid) {
            result += row + '\n';
        }
        return result;
    }    
};

int main() {
    std::vector<std::string> lines = utils::readLinesFromFile(utils::getInputFilePath("inputs/day12.txt"));
    
    /*
    read in shapes before reading in the areas they are supposed to fill. A shape will 
    appear like this in the input file:
    
    0:
    ###
    ##.
    ##.
    */
    std::vector<Shape> shapes;

    bool making_shapes = true;
    std::string currentShape;
    auto line_it = lines.begin();
    while (making_shapes && line_it != lines.end()) {
        const std::string& line = *line_it;
        if (line.empty() && !currentShape.empty()) {
            shapes.push_back(Shape(currentShape));
            currentShape.clear();
        }
        if (std::isdigit(line[0]) && line[1] == ':') {
            making_shapes = true;
            line_it++;
            continue;
        }
        if (std::isdigit(line[0]) && line[1] == 'x') {
            break;
        }

        if (!line.empty() && std::isdigit(line[0]) && line[1] == ':') {
            currentShape += line + '\n';
        }
        else if (line.empty()) {
            shapes.push_back(Shape(currentShape));
            currentShape.clear();
        }
        ++line_it;
    }
    if (!currentShape.empty()) {
        shapes.push_back(Shape(currentShape));
    }

    for (const auto& shape : shapes) {
        std::print("{}", shape.toString());
    }
    return 0;
    std::vector<std::string> areas;

}
