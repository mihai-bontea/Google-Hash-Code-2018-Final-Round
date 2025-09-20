import numpy as np
from dataclasses import dataclass
from typing import List, Tuple

@dataclass
class Building:
    kind: str
    height: int
    width: int
    res_or_type: int
    walls: List[Tuple[int, int]]

def read_building(f) -> Building:
    kind, height, width, res_or_type = f.readline().strip().split()
    height, width, res_or_type = int(height), int(width), int(res_or_type)

    walls = [(i, j)
             for i in range(height)
             for j, ch in enumerate(f.readline().strip())
             if ch == "#"]

    return Building(kind, height, width, res_or_type, walls)


@dataclass
class Solution:
    buildings_placed: int
    id_at_coord: List[Tuple[int, Tuple[int, int]]]

def read_solution(filename) -> Solution:
    with open(filename, 'r') as f:
        buildings_placed = int(f.readline().strip())
        id_at_coord = [(project_id, (i, j)) 
                    for _ in range(buildings_placed)
                    for project_id, i, j in [map(int, f.readline().strip().split())]]
    
    return Solution(buildings_placed, id_at_coord)


def validate_no_building_overlap(buildings, solution):
    walls = []
    for project_id, coords in solution.id_at_coord:
        for wall_i, wall_j in buildings[project_id].walls:
            walls.append((wall_i + coords[0], wall_j + coords[1]))
    
    assert(len(walls) == len(set(walls)))

def get_all_coords_in_range(point, building, walking_distance, height, width):
    coords = set()
    px, py = point

    for cx, cy in building.walls:
        for i_dist in range(-walking_distance, walking_distance + 1):
            i = cx + px + i_dist

            if i < 0 or i >= height:
                continue

            j_limit = walking_distance - abs(i_dist)

            for j_dist in range(-j_limit, j_limit + 1):
                j = cy + py + j_dist

                if j < 0 or j >= width:
                    continue

                coords.add((i, j))

    return coords

def get_unique_utility_types_in_range(city_map, buildings, point, residential_building, walking_distance, height, width):
    coords_to_check = get_all_coords_in_range(point, residential_building, walking_distance, height, width)

    utility_types = set()
    for i, j in coords_to_check:
        if city_map[i, j] != -1:
            utility_types.add(buildings[city_map[i, j]].res_or_type)
    
    return utility_types

def compute_score(height, width, max_walking_dist, buildings, solution):
    # Initialize a height x width map with -1
    city_map = np.full((height, width), -1, dtype=int)

    # Place the utility buildings on the map
    util_ids = [(b_id, coord) for b_id, coord in solution.id_at_coord if buildings[b_id].kind == "U"]
    for utility_id, coords in util_ids:
        building = buildings[utility_id]
        for i, j in building.walls:
            city_map[i + coords[0], j + coords[1]] = utility_id
    
    # For each residential building, get the utility ids in walkable range
    # get the unique utility types; calculate score
    score = 0
    res_ids  = [(b_id, coord) for b_id, coord in solution.id_at_coord if buildings[b_id].kind == "R"]
    for residential_id, coords in res_ids:
        building = buildings[residential_id]

        utility_types = get_unique_utility_types_in_range(city_map, buildings, coords, building, max_walking_dist, height, width)
        score += building.res_or_type * len(utility_types)
    
    return score

solutions = ["sol1"]
input_files = ["a_example", "b_short_walk", "c_going_green", "d_wide_selection", "e_precise_fit", "f_different_footprints"]

for input_file in input_files:
    with open(f"input_files/{input_file}.in", 'r') as f:
        city_height, city_width, max_walking_dist, nr_building_projects = list(map(int, f.readline().split()))
        buildings = [read_building(f) for _ in range(nr_building_projects)]

        for solution in solutions:
            output_file = f"output_files/{solution}/{input_file}.out"
            try:
                solution = read_solution(output_file)

                validate_no_building_overlap(buildings, solution)

            except FileNotFoundError:
                print(f"Error: File '{output_file}' not found.")