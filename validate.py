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