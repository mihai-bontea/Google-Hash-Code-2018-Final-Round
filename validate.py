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
    return {
        (i, j)
        for cx, cy in building.walls
        for i_dist in range(-walking_distance, walking_distance + 1)
        for j_dist in range(-(walking_distance - abs(i_dist)),
                            walking_distance - abs(i_dist) + 1)
        if 0 <= (i := cx + point[0] + i_dist) < height
        if 0 <= (j := cy + point[1] + j_dist) < width
    }

def get_unique_utility_types_in_range(city_map, buildings, point, residential_building, walking_distance, height, width):
    coords_to_check = get_all_coords_in_range(point, residential_building, walking_distance, height, width)
    return {
        buildings[city_map[i, j]].res_or_type
        for i, j in coords_to_check
        if city_map[i, j] != -1
    }

def compute_score(height, width, max_walking_dist, buildings, solution):
    # Initialize a height x width map with -1
    city_map = np.full((height, width), -1, dtype=int)

    # Place the utility buildings on the map
    for utility_id, (x, y) in solution.id_at_coord:
        if buildings[utility_id].kind == "U":
            for i, j in buildings[utility_id].walls:
                city_map[i + x, j + y] = utility_id

    # For each residential building, determine the score based on the amount of unique utility types
    # it has access to within walkable range
    score = nr_utilities = 0
    res_ids  = [(b_id, coord) for b_id, coord in solution.id_at_coord if buildings[b_id].kind == "R"]
    for residential_id, coords in res_ids:
        utility_types = get_unique_utility_types_in_range(
            city_map, buildings, coords, buildings[residential_id], max_walking_dist, height, width)
        nr_utilities += len(utility_types)
        score += buildings[residential_id].res_or_type * len(utility_types)
    
    return score, nr_utilities / len(res_ids)

solutions = ["sol1", "sol2"]
scores = {key: {} for key in solutions}
input_files = ["a_example", "b_short_walk", "c_going_green", "d_wide_selection", "e_precise_fit", "f_different_footprints"]

for input_file in input_files:
    with open(f"input_files/{input_file}.in", 'r') as f:
        city_height, city_width, max_walking_dist, nr_building_projects = list(map(int, f.readline().split()))
        buildings = [read_building(f) for _ in range(nr_building_projects)]

        for sol in solutions:
            output_file = f"output_files/{sol}/{input_file}.out"
            try:
                solution = read_solution(output_file)

                # The buildings in the output file should have no overlapping walls
                validate_no_building_overlap(buildings, solution)

                # Obtain the percentage of the grid covered by walls
                coverage = sum(len(buildings[project_id].walls) for project_id, _ in solution.id_at_coord)
                coverage_percentage = coverage * 100 / (city_height * city_width)

                # Obtain the score and average unique utilities available per residential building
                score, avg_utility_per_res = compute_score(city_height, city_width, max_walking_dist, buildings, solution)
                scores[sol][input_file] = (score, avg_utility_per_res, coverage_percentage)

            except FileNotFoundError:
                print(f"Error: File '{output_file}' not found.")

for solution, input_to_score in scores.items():
    print(f"For {solution}:")

    final_score = 0
    for input_file, (score, avg_utility_per_res, coverage_percentage) in input_to_score.items():
        print(f"--->{input_file}: {score:,} score, {avg_utility_per_res:.1f} average utility access, {coverage_percentage:.1f}% coverage.")
        final_score += score
    print(f"Final score: {final_score:,}.\n")