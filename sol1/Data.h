#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#include <map>

#include <memory>

#include "BuildingProject.h"

#define MAX_BUILDINGS 200

struct Data
{
    int city_height, city_width, max_walking_dist, nr_building_projects;
    std::array<std::unique_ptr<BuildingProject>, MAX_BUILDINGS> buildings;

    Data(const std::string& filename)
    {
        std::ifstream fin(filename);
        fin >> city_height >> city_width >> max_walking_dist >> nr_building_projects;

        auto read_layout = [&fin](int height, int width){
            std::vector<std::pair<int, int>> walls;

            for (int i = 0; i < height; ++i)
                for (int j = 0; j < width; ++j)
                {
                    char c;
                    fin >> c;
                    if (c == '#')
                        walls.emplace_back(i, j);
                }
            return walls;
        };

        char building_type;
        int height, width, res_or_type;
        for (int id = 0; id < nr_building_projects; ++id)
        {
            fin >> building_type >> height >> width >> res_or_type;
            auto walls = read_layout(height, width);
            if (building_type == 'R')
            {
                buildings[id] = std::make_unique<Residential>(height, width, id, res_or_type, walls);
            }
            else
            {
                buildings[id] = std::make_unique<Utility>(height, width, id, res_or_type, walls);
            }
        }
    }

    static void write_to_file(const std::string& filename)
    {
        std::ofstream fout(filename);
    }
};