#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <cmath>
#include <limits>
#include <algorithm>
#include <unordered_set>
#include <utility>

namespace py = pybind11;

struct Edge {
    int source;
    int dest;
    float weight;
};

std::vector<int> bellman_ford(
    int num_nodes,
    const std::vector<Edge>& edges,
    int source,
    std::vector<int>& predecessor
) {
    const float INF = std::numeric_limits<float>::infinity();
    std::vector<float> distances(num_nodes, INF);
    distances[source] = 0;
    predecessor.assign(num_nodes, -1);

    for (int i = 0; i < num_nodes - 1; ++i) {
        for (const auto& edge : edges) {
            if (distances[edge.source] < INF &&
                distances[edge.dest] > distances[edge.source] + edge.weight) {
                distances[edge.dest] = distances[edge.source] + edge.weight;
                predecessor[edge.dest] = edge.source;
            }
        }
    }

    for (const auto& edge : edges) {
        if (distances[edge.source] < INF &&
            distances[edge.dest] > distances[edge.source] + edge.weight) {
            
            std::unordered_set<int> seen;
            int node = edge.dest;
            while (node != -1 && !seen.count(node)) {
                seen.insert(node);
                node = predecessor[node];
            }

            if (node != -1) {
                std::vector<int> cycle;
                int current = node;
                do {
                    cycle.push_back(current);
                    current = predecessor[current];
                } while (current != node && current != -1);
                
                if (current == node) {
                    std::reverse(cycle.begin(), cycle.end());
                    return cycle;
                }
            }
        }
    }

    return {};
}






std::vector<std::pair<std::vector<std::string>, float>> find_arb_ops(
    const std::unordered_map<std::string, 
    std::unordered_map<std::string, float>>& rates
) {
    std::vector<std::pair<std::vector<std::string>, float>> results;

    // Create currency index mapping
    std::vector<std::string> currencies;
    for (const auto& pair : rates) currencies.push_back(pair.first);
    std::unordered_map<std::string, int> currency_index;
    for (size_t i = 0; i < currencies.size(); ++i)
        currency_index[currencies[i]] = i;
    const int num_currencies = currencies.size();

    // Build edge list with negative log weights
    std::vector<Edge> edges;
    for (const auto& [src_curr, targets] : rates) {
        const int src_idx = currency_index[src_curr];
        for (const auto& [dest_curr, rate] : targets) {
            if (rate <= 0) continue;
            const int dest_idx = currency_index[dest_curr];
            edges.push_back({src_idx, dest_idx, -std::log(rate)});
        }
    }

    // Detect and process arbitrage opportunities
    std::unordered_set<std::string> unique_cycles;
    for (int src_idx = 0; src_idx < num_currencies; ++src_idx) {
        std::vector<int> predecessor;
        auto cycle = bellman_ford(num_currencies, edges, src_idx, predecessor);

        if (!cycle.empty()) {
            // Convert indices to currency names
            std::vector<std::string> path;
            for (int idx : cycle) path.push_back(currencies[idx]);

            // Normalize cycle starting point
            auto min_it = std::min_element(path.begin(), path.end());
            std::rotate(path.begin(), min_it, path.end());

            // Create unique key for deduplication
            std::string key;
            for (const auto& curr : path) key += curr + ",";
            
            if (!unique_cycles.count(key)) {
                unique_cycles.insert(key);

                // Calculate actual profit
                float product = 1.0f;
                for (size_t i = 0; i < path.size()-1; ++i)
                    product *= rates.at(path[i]).at(path[i+1]);

                if (product > 1.0001f) {  // Minimum 0.01% threshold
                    results.emplace_back(
                        path,
                        (product - 1.0f) * 100.0f
                    );
                }
            }
        }
    }

    return results;
}

PYBIND11_MODULE(arb_lib, m) {
    m.def("find_arb_ops", &find_arb_ops, "Detect currency arbitrage opportunities");
}