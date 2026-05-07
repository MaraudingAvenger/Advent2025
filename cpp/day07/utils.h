#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace utils {

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(std::move(token));
    }
    return tokens;
}

std::vector<std::string> splitString(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> tokens;
    std::string_view sv{str};
    size_t start = 0;
    size_t end = sv.find(delimiter);
    while (end != std::string_view::npos) {
        tokens.emplace_back(sv.substr(start, end - start));
        start = end + delimiter.length();
        end = sv.find(delimiter, start);
    }
    tokens.emplace_back(sv.substr(start));
    return tokens;
}

std::vector<std::string> splitString(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    for (std::string token; iss >> token; ) {
        tokens.push_back(std::move(token));
    }
    return tokens;
}

std::string joinStrings(const std::vector<std::string>& strings, const std::string& delimiter) {
    if (strings.empty()) {
        return "";
    }
    
    std::string result;
    auto view = strings | std::views::take(strings.size() - 1);
    for (const auto& str : view) {
        result += str;
        result += delimiter;
    }
    result += strings.back();
    return result;
}

std::string joinStrings(const std::vector<std::string>& strings, char delimiter) {
    return joinStrings(strings, std::string(1, delimiter));
}

std::string trim(const std::string& str) {
    constexpr std::string_view whitespace = " \t\n\r";
    std::string_view sv{str};
    
    const auto first = sv.find_first_not_of(whitespace);
    if (first == std::string_view::npos) {
        return "";
    }
    
    const auto last = sv.find_last_not_of(whitespace);
    return std::string{sv.substr(first, last - first + 1)};
}

// File shortcut functions
std::vector<std::filesystem::path> listFilesInDirectory(const std::filesystem::path& dirPath) {
    if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath)) {
        return {};
    }
    
    auto entries = std::filesystem::directory_iterator(dirPath)
        | std::views::filter([](const auto& entry) { return std::filesystem::is_regular_file(entry.status()); })
        | std::views::transform([](const auto& entry) { return entry.path(); });
    
    return std::ranges::to<std::vector<std::filesystem::path>>(entries);
}

std::vector<std::string> readLinesFromFile(const std::filesystem::path& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return {};
    }
    
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(std::move(line));
    }
    
    return lines;
}

std::string readFileToString(const std::filesystem::path& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

}