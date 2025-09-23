#pragma once

#include <map>
#include <memory>
#include <vector>
#include <memory>
#include <fstream>
#include <iostream>

#include "Definitions.h"
#include "BuildingProject.h"

#define MAX_BUILDINGS 200
#define MAX_PROJECT_SIZE 50

struct Data
{
    int city_height, city_width, max_walking_dist, nr_building_projects;
    std::array<std::unique_ptr<BuildingProject>, MAX_BUILDINGS> buildings;

    explicit Data(const std::string& filename)
    {
        std::ifstream fin(filename);
        fin >> city_height >> city_width >> max_walking_dist >> nr_building_projects;

        auto read_layout = [&fin](int height, int width){
            char project_map[MAX_PROJECT_SIZE][MAX_PROJECT_SIZE];

            std::vector<std::pair<int, int>> walls;
            for (int i = 0; i < height; ++i)
                for (int j = 0; j < width; ++j)
                {
                    fin >> project_map[i][j];
                    if (project_map[i][j] == '#')
                        walls.emplace_back(i, j);
                }

            std::vector<std::pair<int, int>> outer_walls;
            for (int i = 0; i < height; ++i)
                for (int j = 0; j < width; ++j)
                    if (project_map[i][j] == '#' && (i == 0 || i == height - 1 || j == 0 || j == width - 1 ||
                    project_map[i - 1][j] == '.' || project_map[i][j - 1] == '.' || project_map[i + 1][j] == '.'
                    || project_map[i][j + 1] == '.'))
                        outer_walls.emplace_back(i, j);

            return std::make_pair(walls, outer_walls);
        };

        char building_type;
        int height, width, res_or_type;
        for (int id = 0; id < nr_building_projects; ++id)
        {
            fin >> building_type >> height >> width >> res_or_type;
            auto [walls, outer_walls] = read_layout(height, width);
            if (building_type == 'R')
            {
                buildings[id] = std::make_unique<Residential>(height, width, id, res_or_type, walls, outer_walls);
            }
            else
            {
                buildings[id] = std::make_unique<Utility>(height, width, id, res_or_type, walls, outer_walls);
            }
        }
    }

    static void write_to_file(const std::string& filename, const std::vector<std::pair<int, Coords>>& chosen_buildings)
    {
        std::ofstream fout(filename);
        fout << chosen_buildings.size() << '\n';
        for (const auto [project_id, coords] : chosen_buildings)
        {
            fout << project_id << " " << coords.first << " " << coords.second << '\n';
        }
    }
};