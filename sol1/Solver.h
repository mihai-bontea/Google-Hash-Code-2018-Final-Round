#pragma once

#include <chrono>
#include <unordered_map>

#include <omp.h>

#include "SimulationState.h"

class Solver
{
private:
    const Data& data;
    SimulationState simulation_state;
    const std::chrono::steady_clock::time_point start;

    /// For the given position, returns a <best_id, best_score> pair, where best_id is the id of
    /// the building that would yield the most points if placed there
    std::pair<int, int> choose_best_building_for_position(Coords point)
    {
        int best_id = -1, best_score = -1;

        omp_set_num_threads(12);
        #pragma omp parallel for
        for (int project_id = 0; project_id < data.nr_building_projects; ++project_id)
        {
            // This building doesn't fit in the given space
            if (!simulation_state.collision_map.can_be_placed(point, project_id))
                continue;

            // Score improvement, save the project id
            const int score_gain = simulation_state.get_points_by_addition(point, project_id);

            #pragma omp critical
            {
                if (score_gain > best_score)
                {
                    best_score = score_gain;
                    best_id = project_id;
                }
            }
        }
        return {best_id, best_score};
    }

    /// Returns a vector containing <project_id, coords> pairs of the chosen buildings
    std::vector<std::pair<int, Coords>> get_processed_results()
    {
        std::vector<std::pair<int, Coords>> results;
        for (const auto [construction_id, coord] : simulation_state.chosen_buildings)
        {
            results.emplace_back(simulation_state.constr_id_to_project_id[construction_id], coord);
        }
        return results;
    }

public:

    explicit Solver(const Data& data)
        : data(data)
        , simulation_state(data)
        , start(std::chrono::steady_clock::now())
    {}

    std::pair<long long, std::vector<std::pair<int, Coords>>> solve()
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

                // Update score and collision map
                simulation_state.total_score += score_increase;

                // Update building states
                simulation_state.add_building({i, j}, best_id);
            }
        }

        const auto now = std::chrono::steady_clock::now();
        const auto elapsed = duration_cast<std::chrono::minutes>(now - start);
        std::cout << "Obtained a solution in " << elapsed.count() << " minutes.\n";

        return {simulation_state.total_score, get_processed_results()};
    }
};