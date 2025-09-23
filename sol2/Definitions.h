#pragma once

#include <cstddef>
#include <climits>
#include <unordered_set>

#define EMPTY SIZE_MAX

using RealIdAndProjectId = std::pair<size_t, int>;
using Coords = std::pair<int, int>;

struct coord_hash {
    std::size_t operator()(const Coords & p) const noexcept {
        return std::hash<int>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
    }
};

inline void hash_combine(std::size_t& seed, std::size_t value) {
    seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct construction_ids_hash {
    std::size_t operator()(const RealIdAndProjectId& p) const noexcept {
        std::size_t seed = 0;
        hash_combine(seed, std::hash<size_t>{}(p.first));
        hash_combine(seed, std::hash<int>{}(p.second));
        return seed;
    }
};

using CoordSet = std::unordered_set<Coords , coord_hash>;
using ConstrIdSet = std::unordered_set<RealIdAndProjectId, construction_ids_hash>;