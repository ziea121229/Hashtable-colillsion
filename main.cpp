#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>
#include "hash_tables.h"

using Clock = std::chrono::high_resolution_clock;

struct RunResult {
    std::string method;
    double target_load_factor;
    std::size_t inserted = 0;
    std::size_t capacity = 0;
    std::size_t collisions = 0;
    std::size_t insert_probes = 0;
    std::size_t search_probes = 0;
    double insert_ms = 0.0;
    double search_ms = 0.0;
    double avg_insert_probes = 0.0;
    double avg_search_probes = 0.0;
    double actual_load_factor = 0.0;
};

std::vector<int> generate_unique_keys(std::size_t count, int seed) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(1, 2000000000);
    std::unordered_set<int> used;
    used.reserve(count * 2);
    std::vector<int> keys;
    keys.reserve(count);

    while (keys.size() < count) {
        int value = dist(rng);
        if (used.insert(value).second) {
            keys.push_back(value);
        }
    }
    return keys;
}

template <typename Table>
RunResult benchmark_table(const std::string& method,
                          double target_load_factor,
                          std::size_t capacity,
                          const std::vector<int>& inserts,
                          const std::vector<int>& searches) {
    Table table(capacity);

    auto insert_start = Clock::now();
    for (int key : inserts) {
        table.insert(key);
    }
    auto insert_end = Clock::now();

    auto search_start = Clock::now();
    for (int key : searches) {
        table.contains(key);
    }
    auto search_end = Clock::now();

    const Metrics& m = table.get_metrics();
    RunResult result;
    result.method = method;
    result.target_load_factor = target_load_factor;
    result.inserted = inserts.size();
    result.capacity = capacity;
    result.collisions = m.collisions;
    result.insert_probes = m.insert_probes;
    result.search_probes = m.search_probes;
    result.insert_ms = std::chrono::duration<double, std::milli>(insert_end - insert_start).count();
    result.search_ms = std::chrono::duration<double, std::milli>(search_end - search_start).count();
    result.avg_insert_probes = static_cast<double>(m.insert_probes) / static_cast<double>(inserts.size());
    result.avg_search_probes = static_cast<double>(m.search_probes) / static_cast<double>(searches.size());
    result.actual_load_factor = table.load_factor();
    return result;
}

void write_csv(const std::vector<RunResult>& results, const std::string& path) {
    std::ofstream out(path);
    out << "method,target_load_factor,actual_load_factor,inserted,capacity,collisions,insert_probes,search_probes,insert_ms,search_ms,avg_insert_probes,avg_search_probes\n";
    out << std::fixed << std::setprecision(6);
    for (const auto& r : results) {
        out << r.method << ','
            << r.target_load_factor << ','
            << r.actual_load_factor << ','
            << r.inserted << ','
            << r.capacity << ','
            << r.collisions << ','
            << r.insert_probes << ','
            << r.search_probes << ','
            << r.insert_ms << ','
            << r.search_ms << ','
            << r.avg_insert_probes << ','
            << r.avg_search_probes << '\n';
    }
}

int main() {
    const std::size_t capacity = 200003;
    const std::vector<double> load_factors = {0.50, 0.70, 0.80, 0.90, 0.95};
    std::vector<RunResult> results;

    for (std::size_t i = 0; i < load_factors.size(); ++i) {
        double alpha = load_factors[i];
        std::size_t n = static_cast<std::size_t>(capacity * alpha);
        auto keys = generate_unique_keys(n, 100 + static_cast<int>(i));

        std::vector<int> searches = keys;
        auto misses = generate_unique_keys(n / 2, 500 + static_cast<int>(i));
        searches.insert(searches.end(), misses.begin(), misses.end());
        std::shuffle(searches.begin(), searches.end(), std::mt19937(900 + static_cast<int>(i)));

        results.push_back(benchmark_table<LinearProbingHashTable>("Linear Probing", alpha, capacity, keys, searches));
        results.push_back(benchmark_table<SeparateChainingHashTable>("Separate Chaining", alpha, capacity, keys, searches));
    }

    write_csv(results, "benchmark_results.csv");

    std::cout << std::left
              << std::setw(20) << "Method"
              << std::setw(10) << "Load"
              << std::setw(14) << "Collisions"
              << std::setw(14) << "Insert(ms)"
              << std::setw(14) << "Search(ms)"
              << std::setw(18) << "AvgInsProbes"
              << std::setw(18) << "AvgSrchProbes"
              << '\n';

    std::cout << std::fixed << std::setprecision(4);
    for (const auto& r : results) {
        std::cout << std::setw(20) << r.method
                  << std::setw(10) << r.target_load_factor
                  << std::setw(14) << r.collisions
                  << std::setw(14) << r.insert_ms
                  << std::setw(14) << r.search_ms
                  << std::setw(18) << r.avg_insert_probes
                  << std::setw(18) << r.avg_search_probes
                  << '\n';
    }

    std::cout << "\nSaved detailed results to benchmark_results.csv\n";
    return 0;
}
