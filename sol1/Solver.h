#pragma once

#include <unordered_map>

#include "Data.h"
#include "CollisionMap.h"

using Point = std::pair<int, int>;

class Solver
{
protected:

public:

    explicit Solver(const Data& data)
            : data(data)
            , collision_map(data)
    {}

    void solve()
    {
        for (int i = 0; i < data.city_height; ++i)
        {
            for (int j = 0; j < data.city_width; ++j)
            {
                // Top left corner is occupied => can't place any building here
                if (!collision_map.is_position_free(i, j))
                    continue;

                // Can be a function
                // =================================================
                int best_score = -1, best_id = -1;
                for (const auto& building_ptr: data.buildings)
                {
                    // This building doesn't fit in the given space
                    if (!collision_map.can_be_placed({i, j}, *(building_ptr.get())))
                        continue;

                    int score_gain = get_points_by_addition({i, j}, building_ptr.get());
                    if (score_gain > best_score)
                    {
                        best_score = score_gain;
                        best_id = building_ptr->id;
                    }
                }
                // ====================================================

                // No building fits here
                if (best_id == -1)
                    continue;

                // Update collision map
                collision_map.place_building({i, j}, *(data.buildings[best_id].get()));

                // Update building states
            }

        }
    }


};