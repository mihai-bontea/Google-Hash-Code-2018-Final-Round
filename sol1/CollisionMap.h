#pragma once

#include <memory>
#include <cstring>
#include <unordered_set>
#include <vector>

#include "BuildingProject.h"
#include "Data.h"

#define EMPTY (-1)

using Point = std::pair<int, int>;

struct pair_hash {
    std::size_t operator()(const Point & p) const noexcept {
        return std::hash<int>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
    }
};

class CollisionMap
{
    const Data& data;
    int height, width, walking_distance;
    std::unique_ptr<std::unique_ptr<int[]>[]> occupant_id;

    std::unordered_set<Point , pair_hash> get_all_coords_in_range(const BuildingProject& building_project) const
    {
        std::unordered_set<Point , pair_hash> coords;
        for (const auto& center : building_project.walls)
        {
            for (int i_dist = -walking_distance; i_dist <= walking_distance; ++i_dist)
            {
                const int i = center.first + i_dist;

                // Invalid row
                if (i < 0 || i >= height)
                    continue;

                const int j_limit = walking_distance - abs(i_dist);

                for (int j_dist = -j_limit; j_dist <= j_limit; ++j_dist)
                {
                    const int j = center.second + j_dist;

                    // Invalid column
                    if (j < 0 || j >= width)
                        continue;

                    coords.emplace(i, j);
                }
            }
        }
        return coords;
    }

    inline bool are_coords_valid(int i, int j) const
    {
        return (i >= 0 && j >= 0 && i < height && j < width);
    }

public:
    explicit CollisionMap(const Data& data)
    : data(data)
    , height(data.city_height)
    , width(data.city_width)
    , walking_distance(data.max_walking_dist)
    {
        occupant_id = std::make_unique<std::unique_ptr<int[]>[]>(height);

        for (int i = 0; i < height; ++i)
        {
            occupant_id[i] = std::make_unique<int[]>(width);

            memset(occupant_id[i].get(), EMPTY, width * sizeof(int));
        }
    }

    bool can_be_placed(Point point, const BuildingProject& building_project)
    {
        for (const auto [i, j] : building_project.walls)
        {
            const int adj_i = i + point.first;
            const int adj_j = j + point.second;

            if (!are_coords_valid(adj_i, adj_j) || occupant_id[adj_i][adj_j] != EMPTY)
                return false;
        }
        return true;
    }

    void place_building(Point point, BuildingProject* building_project)
    {
        for (const auto [i, j] : building_project->walls)
        {
            const int adj_i = i + point.first;
            const int adj_j = j + point.second;

            occupant_id[adj_i][adj_j] = building_project->id;
        }
    }

    void remove_building(Point point, BuildingProject* building_project)
    {
        for (const auto [i, j] : building_project->walls)
        {
            const int adj_i = i + point.first;
            const int adj_j = j + point.second;

            occupant_id[adj_i][adj_j] = EMPTY;
        }
    }

    std::unordered_set<int> get_residential_ids_in_range(const BuildingProject& building_project) const
    {
        const auto coords_within_range = get_all_coords_in_range(building_project);
        std::unordered_set<int> result;

        for (const auto [i, j] : coords_within_range)
        {
            int id = occupant_id[i][j];
            if (id != EMPTY && data.buildings[id]->get_type() == ProjectType::Residential)
                result.emplace(id);
        }
        return result;
    }

    std::unordered_set<int> get_utility_ids_in_range(const BuildingProject& building_project) const
    {
        const auto coords_within_range = get_all_coords_in_range(building_project);
        std::unordered_set<int> result;

        for (const auto [i, j] : coords_within_range)
        {
            int id = occupant_id[i][j];
            if (id != EMPTY && data.buildings[id]->get_type() == ProjectType::Utility)
                result.emplace(id);
        }
        return result;
    }

    inline bool is_position_free(int i, int j) const
    {
        return (occupant_id[i][j] == EMPTY);
    }
};