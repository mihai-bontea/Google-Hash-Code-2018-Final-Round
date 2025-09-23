#pragma once

#include <map>
#include <memory>
#include <vector>
#include <cstring>
#include <algorithm>

#include "Data.h"
#include "BuildingProject.h"

class CollisionMap
{
    const Data& data;
    int height, width, walking_distance;
    std::unique_ptr<std::unique_ptr<RealIdAndProjectId []>[]> occupant_id;
    std::map<int, CoordSet> precomputed_offsets;

    /// For a building project, returns the offsets of the positions around it, within a
    /// radius of walking_distance
    [[nodiscard]] CoordSet get_all_offsets_in_range(const BuildingProject& building_project) const
    {
        CoordSet offsets;

        for (const auto& center: building_project.outer_walls)
        {
            for (int i_dist = -walking_distance; i_dist <= walking_distance; ++i_dist)
            {
                const int i = center.first + i_dist;
                const int j_limit = walking_distance - abs(i_dist);

                for (int j_dist = -j_limit; j_dist <= j_limit; ++j_dist)
                {
                    const int j = center.second + j_dist;
                    offsets.emplace(i, j);
                }
            }
        }
        return offsets;
    }

    /// Computes and stores the offsets of the positions around each building project
    void precompute_offsets()
    {
        for (int project_id = 0; project_id < data.nr_building_projects; ++project_id)
        {
            const auto& building_project = data.buildings[project_id];
            precomputed_offsets[project_id] = get_all_offsets_in_range(*building_project.get());
        }
    }

    /// Returns all coordinates in walking distance of the given building project
    [[nodiscard]] CoordSet get_all_coords_in_range(const Coords& point, const BuildingProject& building_project) const
    {
        CoordSet coords;
        const auto& offsets = precomputed_offsets.find(building_project.id)->second;
        for (const auto [i_off, j_off] : offsets)
        {
            const int i = i_off + point.first;
            const int j = j_off + point.second;

            if (are_coords_valid(i, j))
                coords.emplace(i, j);
        }
        return coords;
    }


    [[nodiscard]] inline bool are_coords_valid(int i, int j) const
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
        occupant_id = std::make_unique<std::unique_ptr<RealIdAndProjectId []>[]>(height);

        for (int i = 0; i < height; ++i)
        {
            occupant_id[i] = std::make_unique<RealIdAndProjectId []>(width);

            for (int j = 0; j < width; ++j)
                occupant_id[i][j] = {EMPTY, 0};
        }
        precompute_offsets();
        std::cout << "Precomputed the offsets for every project.\n";
    }

    /// Returns true if the building project fits at the given point(top left corner)
    /// Collision with other previously placed buildings is taken into account
    [[nodiscard]] bool can_be_placed(const Coords& point, int project_id) const
    {
        const auto& building_project = data.buildings[project_id];
        return std::all_of(building_project->walls.begin(), building_project->walls.end(), [&](const auto& wall){
            const int adj_i = wall.first + point.first;
            const int adj_j = wall.second + point.second;

            return are_coords_valid(adj_i, adj_j) && occupant_id[adj_i][adj_j].first == EMPTY;
        });
    }

    /// Adds a building to the collision map
    void place_building(const Coords& point, RealIdAndProjectId construction_id)
    {
        const auto& building_project = data.buildings[construction_id.second];
        for (const auto [i, j] : building_project->walls)
        {
            const int adj_i = i + point.first;
            const int adj_j = j + point.second;

            occupant_id[adj_i][adj_j] = construction_id;
        }
    }

    /// Removes the building from the collision map
    /// Won't actually be used for the greedy implementation since there is no backtrack step
    void remove_building(const Coords& point, int project_id)
    {
        const auto& building_project = data.buildings[project_id];
        for (const auto [i, j] : building_project->walls)
        {
            const int adj_i = i + point.first;
            const int adj_j = j + point.second;

            occupant_id[adj_i][adj_j] = {EMPTY, 0};
        }
    }

    /// For the given building and coords, returns all residential ids within walkable range
    [[nodiscard]] ConstrIdSet get_residential_ids_in_range(const Coords& point, const BuildingProject& building_project) const
    {
        const auto coords_within_range = get_all_coords_in_range(point, building_project);
        ConstrIdSet result;

        for (const auto [i, j] : coords_within_range)
        {
            auto constr_id = occupant_id[i][j];
            if (constr_id.first != EMPTY && data.buildings[constr_id.second]->get_type() == ProjectType::Residential)
                result.emplace(constr_id);
        }
        return result;
    }

    /// For the given building and coords, returns all utility ids within walkable range
    [[nodiscard]] ConstrIdSet get_utility_ids_in_range(const Coords& point, const BuildingProject& building_project) const
    {
        const auto coords_within_range = get_all_coords_in_range(point, building_project);
        ConstrIdSet result;

        for (const auto [i, j] : coords_within_range)
        {
            auto constr_id = occupant_id[i][j];
            if (constr_id.first != EMPTY && data.buildings[constr_id.second]->get_type() == ProjectType::Utility)
                result.emplace(constr_id);
        }
        return result;
    }

    [[nodiscard]] inline bool is_position_free(int i, int j) const
    {
        return (occupant_id[i][j].first == EMPTY);
    }
};