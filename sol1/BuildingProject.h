#pragma once

#include <unordered_set>

#define MAX_TYPES_UTILITY 200

enum class ProjectType{
    Residential,
    Utility,
    Unknown
};

struct BuildingProject {
    const int height, width, id;
    const std::vector<std::pair<int, int>> walls;

    BuildingProject(int height, int width, int id, std::vector<std::pair<int, int>> walls)
    : height(height)
    , width(width)
    , id(id)
    , walls(std::move(walls))
    {}

    [[nodiscard]] virtual ProjectType get_type() const
    {
        return ProjectType::Unknown;
    }

    virtual ~BuildingProject() = default;
};

struct Residential : public BuildingProject{
    int nr_residents;
//    std::vector<std::unordered_set<int>> util_type_to_ids;

    Residential(int height, int width, int id, int nr_residents, std::vector<std::pair<int, int>> walls)
    : BuildingProject(height, width, id, std::move(walls))
    , nr_residents(nr_residents)
    {
//        util_type_to_ids.reserve(MAX_TYPES_UTILITY);
    }

    [[nodiscard]] ProjectType get_type() const override
    {
        return ProjectType::Residential;
    }
};

struct Utility : public BuildingProject {
    int utility_type;

    Utility(int height, int width, int id, int utility_type, std::vector<std::pair<int, int>> walls)
    : BuildingProject(height, width, id, std::move(walls))
    , utility_type(utility_type)
    {}

    [[nodiscard]] ProjectType get_type() const override
    {
        return ProjectType::Utility;
    }
};