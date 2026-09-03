#include <cstring>
#include <iostream>
#include <print>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "../common/utils.h"


class Node {
    public:
    char value[4];
    
    Node(const char* v): value{0} { std::strncpy(value, v, sizeof(value) - 1); }
};

struct PairHash {
    std::size_t operator()(const std::pair<Node*, Node*>& p) const {
        return std::hash<Node*>()(p.first) ^ std::hash<Node*>()(p.second);
    }
};

class Graph {
public:
    std::unordered_set<Node*> nodes;
    std::unordered_map<Node*, std::unordered_set<Node*>> adjacencyList;

    Graph() : nodes{}, adjacencyList{} {}
};

Node* getNode(Graph& graph, const char* value) {
    for (Node* node : graph.nodes) {
        if (std::strncmp(node->value, value, sizeof(node->value)) == 0) {
            return node;
        }
    }
    Node* newNode = new Node(value);
    graph.nodes.insert(newNode);
    return newNode;
}

long countPaths(const Graph& graph,
                Node* cur,
                Node* end,
                std::unordered_map<Node*, long>& memo) {
    if (cur == end) return 1;
    if (auto it = memo.find(cur); it != memo.end()) return it->second;

    long total = 0;
    if (auto it = graph.adjacencyList.find(cur); it != graph.adjacencyList.end()) {
        for (Node* next : it->second) {
            total += countPaths(graph, next, end, memo);
        }
    }
    return memo[cur] = total;
}

long countPaths(const Graph& graph, Node* from, Node* to) {
    std::unordered_map<Node*, long> memo;
    return countPaths(graph, from, to, memo);
}

int main() {
    const auto& lines = utils::readLinesFromFile(utils::getInputFilePath("inputs/day11.txt"));
    Graph graph;

    for (const auto& line : lines) {
        auto parts = utils::splitString(utils::replace(line, ':', ' '));
        Node* parent = getNode(graph, parts[0].c_str());
        for (size_t i = 1; i < parts.size(); ++i) {
            Node* child = getNode(graph, parts[i].c_str());
            graph.adjacencyList[parent].insert(child);
        }
    }

    Node* you = getNode(graph, "you");
    Node* out = getNode(graph, "out");
    
    long part1 = countPaths(graph, you, out);
    std::println("Part 1: {}", part1);
    
    Node* svr = getNode(graph, "svr");
    Node* dac = getNode(graph, "dac");
    Node* fft = getNode(graph, "fft");

    long part2 =
        countPaths(graph, svr, dac) * countPaths(graph, dac, fft) * countPaths(graph, fft, out)
      + countPaths(graph, svr, fft) * countPaths(graph, fft, dac) * countPaths(graph, dac, out);
    std::println("Part 2: {}", part2);
}