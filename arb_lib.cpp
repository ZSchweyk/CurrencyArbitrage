#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // Required for STL container conversions
#include <vector>
#include <utility> // for std::pair
#include <unordered_map>
#include <string>
#include <iostream>

using std::vector;
using std::pair;
using std::string;
using std::unordered_map;
using std::cout;
using std::endl;

namespace py = pybind11;



// Arbitrage detection logic (simplified example)
vector<pair<vector<string>, float>> find_arb_ops(
    const unordered_map<string, unordered_map<string, float>>& rates
) {
    vector<pair<vector<string>, float>> opportunities;
    
    // // Simplified example: Replace with Bellman-Ford/Floyd-Warshall logic
    // // For demo, assume we found one arbitrage path:
    // opportunities.push_back({{"usd", "eur", "brl", "usd"}, 1.05f});
    
    // return opportunities;

	cout << "In cpp function" << endl;

	return opportunities;
}



PYBIND11_MODULE(arb_lib, m) {
	m.def("find_arb_ops", &find_arb_ops, "Find Arbitrage Opportunites");
}