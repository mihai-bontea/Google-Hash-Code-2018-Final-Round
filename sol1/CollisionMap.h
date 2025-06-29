#pragma once

#include <memory>
#include <cstring>
#include <unordered_set>
#include <vector>

#include "BuildingProject.h"
#include "Data.h"

#define EMPTY (-1)

using Point = std::pair<int, int>;

class CollisionMap
{
    const Data& data;
    int height, width, walking_distance;
    std::unique_ptr<std::unique_ptr<int[]>[]> occupant_id;

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

    inline bool are_coords_valid(int i, int j) const
    {
        return (i >= 0 && j >= 0 && i < height && j < width);
    }

    bool can_be_placed(Point point, BuildingProject* building_project)
    {
        for (const auto [i, j] : building_project->walls)
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

    std::unordered_set<int> get_all_building_ids_in_range(Point center)
    {
        std::unordered_set<int> building_ids;

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

                if (occupant_id[i][j] != EMPTY)
                    building_ids.insert(occupant_id[i][j]);
            }

        }
        return building_ids;
    }

    std::vector<int> get_residential_ids_in_range(Point center)
    {
        const auto all_building_ids = get_all_building_ids_in_range(center);
        std::vector<int> result;

        for (int id : all_building_ids)
        {
            if (data.buildings[id].get().get_type() == ProjectType::Residential)
                result.push_back(id);
        }
        return result;
    }

    std::vector<int> get_utility_ids_in_range(Point center)
    {
        const auto all_building_ids = get_all_building_ids_in_range(center);
        std::vector<int> result;

        for (int id : all_building_ids)
        {
            if (data.buildings[id].get().get_type() == ProjectType::Utility)
                result.push_back(id);
        }
        return result;
    }
};