#include <iostream>
#include <array>

#include "Data.h"
#include "CollisionMap.h"

class Solver
{
    const Data& data;
    CollisionMap collision_map;
//    unsigned long long score;

public:

    explicit Solver(const Data& data)
    : data(data)
    , collision_map(data)
//    , score(0)
    {}

    void solve()
    {
        for (int i = 0; i < data.city_height; ++i)
        {
            for (int j = 0; j < data.city_width; ++j)
                if (collision_map.is_position_free(i, j))
                {
                    for (const auto& building_ptr : data.buildings)
                    {
                        if (collision_map.can_be_placed({i, j}, *(building_ptr.get())))
                        {

                        }
                    }


                }
        }
    }

    int get_points_by_addition(const Residential& residential)
    {
        auto utility_ids_within_range = collision_map.get_utility_ids_in_range(residential);
        auto utility_types_within_range = get_utility_types(utility_ids_within_range);

        int score_added = utility_types_within_range.size() * residential.nr_residents;
        return score_added;
    }

    int get_points_by_addition(const BuildingProject& building_project, Point point)
    {
        if (building_project.get_type() == ProjectType::Residential)
        {
//            const auto residential_ptr = dynamic_cast<const Residential*>(&building_project);

//            auto utility_ids_within_range = collision_map.get_utility_ids_in_range(building_project);
//            auto utility_types_within_range = get_utility_types(utility_ids_within_range);
//
//            int score_added = utility_types_within_range.size() * residential_ptr->nr_residents;
        }
        else
        {
            const auto utility_ptr = dynamic_cast<const Utility*>(&building_project);

            auto residential_ids_within_range = collision_map.get_residential_ids_in_range(building_project);


        }
    }

    std::unordered_set<int> get_utility_types(const std::unordered_set<int>& utility_ids)
    {
        std::unordered_set<int> result;
        for (int id : utility_ids)
        {
            auto residential_ptr = dynamic_cast<Utility*>(data.buildings[id].get());
            result.insert(residential_ptr->utility_type);
        }
        return result;
    }
};

int main()
{
    const std::string in_prefix = "../../input_files/";
    const std::string out_prefix = "../../output_files/sol1/";
    const std::array<std::string, 6> input_files = {"a_example.in", "b_short_walk.in", "c_going_green.in",
                                                    "d_wide_selection.in", "e_precise_fit.in", "f_different_footprints.in"};

    for (const auto& input_file : input_files)
    {
        Data data(in_prefix + input_file);
        std::cout << "Successfully read\n";
//        std::cout << "max walking distance = " << data.max_walking_dist << std::endl;
        Solver solver(data);


        const auto out_filename = out_prefix + input_file.substr(0, (input_file.find('.'))) + ".out";
        Data::write_to_file(out_filename);
    }

    return 0;
}
