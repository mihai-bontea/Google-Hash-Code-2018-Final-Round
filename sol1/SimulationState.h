#pragma once

#include <unordered_map>

#include "CollisionMap.h"

class SimulationState
{
private:
    const Data& data;

    size_t next_constr_id;
    std::unordered_map<size_t, int> constr_id_to_project_id;
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
    int get_points_by_addition(const Coords& point, const Utility& utility)
    {
        const auto residential_ids_within_range = collision_map.get_residential_ids_in_range(point, utility);
//        std::cout << "resuming point calc for utility\n";

        // Go over each residential in range, check if their utility type is satisfied
        int score_added = 0;
        for (auto [real_id, residential_id] : residential_ids_within_range)
        {
//            std::cout << "real_id= " << real_id << " residential_id= " << residential_id << std::endl;
            const auto residential_ptr = dynamic_cast<const Residential*>(data.buildings[residential_id].get());
            auto& utils_by_type = res_id_to_utility_by_type[real_id];

            // This utility type is unsatisfied => score increase
            if (utils_by_type[utility.utility_type].empty())
                score_added += residential_ptr->nr_residents;
        }
        return score_added;
    }

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

    explicit SimulationState(const Data& data)
        : data(data)
        , collision_map(data)
        , next_constr_id(0)
    {}

    int get_points_by_addition(const Coords& point, int project_id)
    {
        const auto& building_project = data.buildings[project_id].get();
        if (building_project->get_type() == ProjectType::Residential)
        {
//            std::cout << "checking residential\n";
            const auto residential_ptr = dynamic_cast<const Residential*>(building_project);
            return get_points_by_addition(point, *residential_ptr);
        }
        else
        {
//            std::cout << "checking utility\n";
            const auto utility_ptr = dynamic_cast<const Utility*>(building_project);
            return get_points_by_addition(point, *utility_ptr);
        }
    }

    void add_building(const Coords& point, int project_id)
    {
        // Store the top-left corner of the building
        chosen_buildings[next_constr_id] = point;

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
};