#pragma once

#include <unordered_map>

#include "CollisionMap.h"

class SimulationState
{
private:
    const Data& data;
    size_t next_constr_id;
    std::unordered_map<size_t, std::unordered_map<int, std::vector<size_t>>> res_id_to_utility_by_type;

    std::unordered_set<int> get_utility_types(const ConstrIdSet& utility_ids)
    {
        std::unordered_set<int> result;
        for (auto [_, project_id] : utility_ids)
        {
            auto residential_ptr = dynamic_cast<Utility*>(data.buildings[project_id].get());
            result.insert(residential_ptr->utility_type);
        }
        return result;
    }

    /// When adding a residential building, the score increase is determined by its nr_residents
    /// multiplied by the unique utility types in walkable range from it
    int get_points_by_addition(const Coords& point, const Residential& residential)
    {
        const auto utility_ids_within_range = collision_map.get_utility_ids_in_range(point, residential);
        const auto utility_types_within_range = get_utility_types(utility_ids_within_range);

        int score_added = (int)utility_types_within_range.size() * residential.nr_residents;
        return score_added;
    }

    /// When adding a utility building, the score increase is determined by the number of residents around
    /// who are now receiving access to a NEW utility type(no increase if they already had access to this type)
    int get_points_by_addition(const Coords& point, const Utility& utility) const
    {
        const auto residential_ids_within_range = collision_map.get_residential_ids_in_range(point, utility);

        // Go over each residential in range, check if their utility type is satisfied
        int score_added = 0;
        for (auto [real_id, residential_id] : residential_ids_within_range)
        {
            const auto residential_ptr = dynamic_cast<const Residential*>(data.buildings[residential_id].get());
            const auto utils_by_type_it = res_id_to_utility_by_type.find(real_id);
            if (utils_by_type_it == res_id_to_utility_by_type.end())
                continue;

            // This utility type is unsatisfied => score increase
            auto utility_type_it = utils_by_type_it->second.find(utility.utility_type);
            if (utility_type_it ==  utils_by_type_it->second.end() || utility_type_it->second.empty())
                score_added += residential_ptr->nr_residents;
        }
        return score_added;
    }

    /// Adds a residential building to the collision map, updates the score, and stores the coords
    void add_building(const Coords& point, const Residential& residential)
    {
        const auto utility_ids_within_range = collision_map.get_utility_ids_in_range(point, residential);
        auto& utility_by_type = res_id_to_utility_by_type[next_constr_id];
        for (auto [real_id, utility_id] : utility_ids_within_range)
        {
            auto utility_ptr = dynamic_cast<Utility*>(data.buildings[utility_id].get());
            utility_by_type[utility_ptr->utility_type].push_back(real_id);
        }
    }

    /// Adds a utility building to the collision map, updates the score, and stores the coords
    void add_building(const Coords& point, const Utility& utility)
    {
        const auto residential_ids_within_range = collision_map.get_residential_ids_in_range(point, utility);
        for (auto [real_id, residential_id] : residential_ids_within_range)
        {
            auto& utility_by_type = res_id_to_utility_by_type[real_id];
            utility_by_type[utility.utility_type].push_back(next_constr_id);
        }
    }

public:
    CollisionMap collision_map;
    unsigned long long total_score = 0;
    std::unordered_map<size_t, Coords> chosen_buildings;
    std::unordered_map<size_t, int> constr_id_to_project_id, id_to_score_gained;

    explicit SimulationState(const Data& data)
            : data(data)
            , collision_map(data)
            , next_constr_id(0)
    {}

    /// Returns the points gained by adding the building at the given coords
    int get_points_by_addition(const Coords& point, int project_id)
    {
        const auto& building_project = data.buildings[project_id].get();
        if (building_project->get_type() == ProjectType::Residential)
        {
            const auto residential_ptr = dynamic_cast<const Residential*>(building_project);
            return get_points_by_addition(point, *residential_ptr);
        }
        else
        {
            const auto utility_ptr = dynamic_cast<const Utility*>(building_project);
            return get_points_by_addition(point, *utility_ptr);
        }
    }

    /// Adds a building to the collision map, updates the score, and stores the coords
    void add_building(const Coords& point, int project_id, int score_gained)
    {
        // Store the top-left corner of the building
        chosen_buildings[next_constr_id] = point;

        // Store the score increase from this project, and update total score
        id_to_score_gained[next_constr_id] = score_gained;
        total_score += score_gained;

        // Link the real construction id to the project_id
        constr_id_to_project_id[next_constr_id] = project_id;

        // Add it to the collision map
        collision_map.place_building(point, {next_constr_id, project_id});

        const auto& building_project = data.buildings[project_id].get();
        if (building_project->get_type() == ProjectType::Residential)
        {
            const auto residential_ptr = dynamic_cast<const Residential*>(building_project);
            add_building(point, *residential_ptr);
        }
        else
        {
            const auto utility_ptr = dynamic_cast<const Utility*>(building_project);
            add_building(point, *utility_ptr);
        }

        // Increment the id
        ++next_constr_id;
    }

    /// Removes a residential building to the collision map, updates the score, and stores the coords
    void remove_building(const Coords& point, size_t construction_id)
    {
        const int project_id = constr_id_to_project_id[construction_id];
        const auto& building_project = data.buildings[project_id].get();
        if (building_project->get_type() == ProjectType::Residential)
        {
            res_id_to_utility_by_type.erase(construction_id);
        }
        else
        {
            const auto utility_ptr = dynamic_cast<const Utility*>(building_project);
            const auto residential_ids_within_range = collision_map.get_residential_ids_in_range(point, *utility_ptr);
            for (auto [real_id, residential_id] : residential_ids_within_range)
            {
                auto& utility_by_type = res_id_to_utility_by_type[real_id];
                auto& utility_for_type = utility_by_type[utility_ptr->utility_type];
                utility_for_type.erase(std::remove(utility_for_type.begin(), utility_for_type.end(), construction_id));
            }

        }
        // Update score
        total_score -= id_to_score_gained[construction_id];

        Coords coords = chosen_buildings[construction_id];
        chosen_buildings.erase(construction_id);

        id_to_score_gained.erase(construction_id);
        constr_id_to_project_id.erase(construction_id);

        collision_map.remove_building(coords, project_id);
    }
};