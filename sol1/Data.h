#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#include <map>

#include "BuildingProject.h"

struct Data
{
    int city_height, city_width, max_walking_dist, nr_building_projects;
    std::vector<Residential> residential_projects;
    std::map<int, std::vector<Utility>> type_to_utility;

    Data(const std::string& filename)
    {
        std::ifstream fin(filename);
        fin >> city_height >> city_width >> max_walking_dist >> nr_building_projects;

        auto read_layout = [&fin](auto& building, int height, int width){
            for (int i = 0; i < height; ++i)
                for (int j = 0; j < width; ++j)
                {
                    char c;
                    fin >> c;
                    building.set(i, j, static_cast<CellType>(c == '#'));
                }
        };

        char building_type;
        int height, width, res_or_type;
        for (int id = 0; id < nr_building_projects; ++id)
        {
            fin >> building_type >> height >> width >> res_or_type;
            if (building_type == 'R')
            {
                Residential residential(height, width, id, res_or_type);
                read_layout(residential, height, width);
                residential_projects.push_back(residential);
            }
            else
            {
                Utility utility(height, width, id, res_or_type);
                read_layout(utility, height, width);
                type_to_utility[res_or_type].push_back(utility);
            }
        }
    }

    static void write_to_file(const std::string& filename)
    {
        std::ofstream fout(filename);
    }
};