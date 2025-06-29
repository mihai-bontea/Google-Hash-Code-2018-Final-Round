#pragma once

#include <memory>
#include <cstring>

#include "BuildingProject.h"

#define EMPTY (-1)
#define MAX_GRID_SIZE 1000

using Point = std::pair<int, int>;

class CollisionMap
{
    std::unique_ptr<std::unique_ptr<int[]>[]> occupant_id;

public:
    CollisionMap()
    {
        occupant_id = std::make_unique<std::unique_ptr<int[]>[]>(MAX_GRID_SIZE);

        for (int i = 0; i < MAX_GRID_SIZE; ++i)
        {
            occupant_id[i] = std::make_unique<int[]>(MAX_GRID_SIZE);

            memset(occupant_id[i].get(), EMPTY, MAX_GRID_SIZE * sizeof(int));
        }
    }

    bool can_be_placed(Point point, BuildingProject* building_project)
    {
        
    }
};