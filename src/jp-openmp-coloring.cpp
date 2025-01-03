#include <algorithm>
#include <iostream>
#include <unordered_set>
#include "graph.h"

class JPOpenMPColorGraph : public ColorGraph {
public:
  void buildGraph(std::vector<graphNode> &nodes, std::vector<std::pair<int, int>> &pairs,
                  std::unordered_map<graphNode, std::vector<graphNode>> &graph) {
    for (auto &node : nodes) {
      graph[node] = {};
    }
  
    size_t numPairs = pairs.size();

    for (size_t i = 0; i < numPairs; i++) {
      int first = pairs[i].first;
      int second = pairs[i].second;

      graph[first].push_back(second);

      graph[second].push_back(first);
    }
  }

  int firstAvailableColor(int node, std::unordered_map<graphNode, std::vector<graphNode>> &graph,
                          std::unordered_map<graphNode, color> &colors) {
    std::unordered_set<int> usedColors;
    for (const auto &nbor : graph[node]) {
      if (colors[nbor] != -1) {
        usedColors.insert(colors[nbor]);
      }
    }

    int minColor = 0;
    while(true) {
      if (usedColors.find(minColor) == usedColors.end()) {
        return minColor;
      }
      minColor++;
    }
  }

  void colorGraph(std::unordered_map<graphNode, std::vector<graphNode>> &graph,
                  std::unordered_map<graphNode, color> &colors) {
   
    int numNodes = (int) graph.size(); 
    for (int i = 0; i < numNodes; i++) {
      colors[i] = -1;
    }

    int numMarked = 0;
        
    while (numMarked < numNodes) {
      #pragma omp parallel for schedule(dynamic, 2) shared(graph, colors, numMarked)
      for (int i = 0; i < numNodes; i++) {
        if (colors[i] == -1) {
          bool colorNow = true;
          for (const auto &nbor : graph[i]) {
            if (colors[nbor] == -1 && i < nbor) {
              colorNow = false;
              break;
            }
          }
          if (colorNow) {
            colors[i] = firstAvailableColor(i, graph, colors);
            #pragma omp atomic
            numMarked++;
          }
        }
      }
    }
  }
};

std::unique_ptr<ColorGraph> createJPOpenMPColorGraph() {
  return std::make_unique<JPOpenMPColorGraph>();
}
