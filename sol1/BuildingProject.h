#pragma once

#include <bitset>

#define MAX_PROJECT_DIM 50
#define MAX_PROJECT_AREA 2500

enum class CellType : bool {
    Empty = false,
    Wall  = true
};

struct BuildingProject {
    int height, width, id;
    std::bitset<MAX_PROJECT_AREA> plan;

    BuildingProject() = default;

    BuildingProject(int height, int width, int id)
            : height(height), width(width), id(id) {
        plan.reset();
    }

    inline CellType at(int row, int col) const {
        return static_cast<CellType>(plan[row * MAX_PROJECT_DIM + col]);
    }

    inline void set(int row, int col, CellType value) {
        plan[row * MAX_PROJECT_DIM + col] = static_cast<bool>(value);
    }

    virtual ~BuildingProject() = default;
};

struct Residential : public BuildingProject {
    int nr_residents;

    Residential(int height, int width, int id, int nr_residents)
            : BuildingProject(height, width, id), nr_residents(nr_residents) {}
};

struct Utility : public BuildingProject {
    int utility_type;

    Utility(int height, int width, int id, int utility_type)
            : BuildingProject(height, width, id), utility_type(utility_type) {}
};