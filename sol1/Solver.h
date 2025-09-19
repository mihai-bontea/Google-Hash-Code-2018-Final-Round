#pragma once

#include <unordered_map>

#include "SimulationState.h"

using Point = std::pair<int, int>;

class Solver
{
protected:
    const Data& data;
    SimulationState simulation_state;

public:

    explicit Solver(const Data& data)
            : data(data)
            , simulation_state(data)
    {}

    std::pair<int, int> choose_best_building_for_position(Point point)
    {
//        std::cout << "Trying to choose a building\n";
        int best_id = -1, best_score = -1;

        for (int project_id = 0; project_id < data.nr_building_projects; ++project_id)
        {
//            std::cout << "trying to place " << project_id << "\n";
            // This building doesn't fit in the given space
            if (!simulation_state.collision_map.can_be_placed(point, project_id))
                continue;

            // Score improvement, save the project id
            const int score_gain = simulation_state.get_points_by_addition(point, project_id);
//            std::cout << "score_gain = " << score_gain << std::endl;
            if (score_gain > best_score)
            {
                best_score = score_gain;
                best_id = project_id;
            }
        }
//        std::cout << "best_id = " << best_id << " best_score = " << best_score << std::endl;
        return {best_id, best_score};
    }

    void solve()
    {
        for (int i = 0; i < data.city_height; ++i)
        {
            for (int j = 0; j < data.city_width; ++j)
            {
                // Top left corner is occupied => can't place any building here
                if (!simulation_state.collision_map.is_position_free(i, j))
                    continue;

                // Find the building that would yield the highest score for this position
                const auto [best_id, score_increase] = choose_best_building_for_position({i, j});

                // No building fits here, move on
                if (best_id == -1)
                    continue;

//                std::cout << "Placing a building\n";
                // Update score and collision map
                simulation_state.total_score += score_increase;

                // Update building states
                simulation_state.add_building({i, j}, best_id);
            }
        }

        std::cout << "Placed " << simulation_state.chosen_buildings.size() << " buildings. Score = "
            << simulation_state.total_score << std::endl;
    }
};