#pragma once

#include <memory>
#include <cstring>

#include "BuildingProject.h"

#define EMPTY (-1)

using Point = std::pair<int, int>;

class CollisionMap
{
    int height, width;
    std::unique_ptr<std::unique_ptr<int[]>[]> occupant_id;

public:
    CollisionMap(int height, int width)
    : height(height)
    , width(width)
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
};