#ifndef HASH_TABLES_H
#define HASH_TABLES_H

#include <vector>
#include <list>
#include <optional>
#include <functional>
#include <stdexcept>
#include <cstddef>

struct Metrics {
    std::size_t collisions = 0;
    std::size_t insert_probes = 0;
    std::size_t search_probes = 0;
    std::size_t successful_searches = 0;
    std::size_t unsuccessful_searches = 0;
};

class LinearProbingHashTable {
private:
    enum class SlotState { EMPTY, OCCUPIED, DELETED };

    struct Slot {
        int key = 0;
        SlotState state = SlotState::EMPTY;
    };

    std::vector<Slot> table;
    std::size_t current_size;
    Metrics metrics;

    std::size_t index_for(int key) const {
        return std::hash<int>{}(key) % table.size();
    }

public:
    explicit LinearProbingHashTable(std::size_t capacity)
        : table(capacity), current_size(0) {
        if (capacity == 0) {
            throw std::invalid_argument("Capacity must be positive.");
        }
    }

    bool insert(int key) {
        if (current_size >= table.size()) {
            return false;
        }

        std::size_t start = index_for(key);
        for (std::size_t i = 0; i < table.size(); ++i) {
            std::size_t idx = (start + i) % table.size();
            metrics.insert_probes++;

            if (table[idx].state == SlotState::OCCUPIED) {
                if (table[idx].key == key) {
                    return false;
                }
                metrics.collisions++;
                continue;
            }

            table[idx].key = key;
            table[idx].state = SlotState::OCCUPIED;
            current_size++;
            return true;
        }
        return false;
    }

    bool contains(int key) {
        std::size_t start = index_for(key);
        for (std::size_t i = 0; i < table.size(); ++i) {
            std::size_t idx = (start + i) % table.size();
            metrics.search_probes++;

            if (table[idx].state == SlotState::EMPTY) {
                metrics.unsuccessful_searches++;
                return false;
            }
            if (table[idx].state == SlotState::OCCUPIED && table[idx].key == key) {
                metrics.successful_searches++;
                return true;
            }
        }
        metrics.unsuccessful_searches++;
        return false;
    }

    double load_factor() const {
        return static_cast<double>(current_size) / static_cast<double>(table.size());
    }

    std::size_t size() const { return current_size; }
    std::size_t capacity() const { return table.size(); }
    const Metrics& get_metrics() const { return metrics; }
};

class SeparateChainingHashTable {
private:
    std::vector<std::list<int>> table;
    std::size_t current_size;
    Metrics metrics;

    std::size_t index_for(int key) const {
        return std::hash<int>{}(key) % table.size();
    }

public:
    explicit SeparateChainingHashTable(std::size_t capacity)
        : table(capacity), current_size(0) {
        if (capacity == 0) {
            throw std::invalid_argument("Capacity must be positive.");
        }
    }

    bool insert(int key) {
        std::size_t idx = index_for(key);
        metrics.insert_probes++;

        if (!table[idx].empty()) {
            metrics.collisions++;
        }

        for (int existing : table[idx]) {
            metrics.insert_probes++;
            if (existing == key) {
                return false;
            }
        }

        table[idx].push_back(key);
        current_size++;
        return true;
    }

    bool contains(int key) {
        std::size_t idx = index_for(key);
        for (int existing : table[idx]) {
            metrics.search_probes++;
            if (existing == key) {
                metrics.successful_searches++;
                return true;
            }
        }
        metrics.search_probes++;
        metrics.unsuccessful_searches++;
        return false;
    }

    double load_factor() const {
        return static_cast<double>(current_size) / static_cast<double>(table.size());
    }

    std::size_t size() const { return current_size; }
    std::size_t capacity() const { return table.size(); }
    const Metrics& get_metrics() const { return metrics; }
};

#endif
