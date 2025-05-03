#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <cmath>
#include <limits>
#include <algorithm>

namespace py = pybind11;

// Structure to represent edges
struct Edge {
    int source;
    int dest;
    float weight;
};

// Bellman-Ford implementation with negative cycle detection
std::vector<int> bellman_ford(
    int num_nodes,
    const std::vector<Edge>& edges,
    int source,
    std::vector<int>& predecessor
) {
    const float INF = std::numeric_limits<float>::infinity();
    std::vector<float> distances(num_nodes, INF);
    distances[source] = 0;
    predecessor.resize(num_nodes, -1);

    // Relax all edges V-1 times
    for (int i = 0; i < num_nodes - 1; ++i) {
        for (const auto& edge : edges) {
            if (distances[edge.source] < INF && 
                distances[edge.dest] > distances[edge.source] + edge.weight) {
                distances[edge.dest] = distances[edge.source] + edge.weight;
                predecessor[edge.dest] = edge.source;
            }
        }
    }

    // Check for negative cycles
    for (const auto& edge : edges) {
        if (distances[edge.source] < INF && 
            distances[edge.dest] > distances[edge.source] + edge.weight) {
            // Negative cycle found, trace back to find the path
            std::vector<int> cycle;
            int node = edge.dest;
            
            // Walk back num_nodes steps to ensure we're in the cycle
            for (int i = 0; i < num_nodes; ++i) {
                node = predecessor[node];
            }

            // Reconstruct the cycle
            int current = node;
            do {
                cycle.push_back(current);
                current = predecessor[current];
            } while (current != node && current != -1);

            if (current == node) {
                cycle.push_back(node);
                std::reverse(cycle.begin(), cycle.end());
                return cycle;
            }
        }
    }

    return {};
}

// Main arbitrage detection function
std::vector<std::pair<std::vector<std::string>, float>> find_arb_ops(
    const std::unordered_map<std::string, 
    std::unordered_map<std::string, float>>& rates
) {
    std::vector<std::pair<std::vector<std::string>, float>> results;

    // Create currency index mapping
    std::vector<std::string> currencies;
    for (const auto& pair : rates) {
        currencies.push_back(pair.first);
    }
    std::unordered_map<std::string, int> currency_index;
    for (size_t i = 0; i < currencies.size(); ++i) {
        currency_index[currencies[i]] = i;
    }
    const int num_currencies = currencies.size();

    // Build edge list with negative log weights
    std::vector<Edge> edges;
    for (const auto& [src_curr, targets] : rates) {
        const int src_idx = currency_index[src_curr];
        for (const auto& [dest_curr, rate] : targets) {
            const int dest_idx = currency_index[dest_curr];
            if (rate > 0) {
                edges.push_back({src_idx, dest_idx, -std::log(rate)});
            }
        }
    }

    // Detect arbitrage for each potential starting currency
    for (int src_idx = 0; src_idx < num_currencies; ++src_idx) {
        std::vector<int> predecessor;
        auto cycle = bellman_ford(num_currencies, edges, src_idx, predecessor);

        if (!cycle.empty()) {
            // Calculate actual profit from exchange rates
            float product = 1.0f;
            std::vector<std::string> path;
            
            for (size_t i = 0; i < cycle.size() - 1; ++i) {
                const std::string from = currencies[cycle[i]];
                const std::string to = currencies[cycle[i+1]];
                product *= rates.at(from).at(to);
                path.push_back(from);
            }
            path.push_back(currencies[cycle.back()]);

            if (product > 1.0f) {
                results.emplace_back(
                    path, 
                    (product - 1.0f) * 100.0f  // Convert to percentage
                );
            }
        }
    }

    return results;
}

PYBIND11_MODULE(arb_lib, m) {
    m.def("find_arb_ops", &find_arb_ops, "Detect currency arbitrage opportunities");
}